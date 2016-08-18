/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/
#include "StringUtil.h"
#include<cstdio>
#include<iostream>
#include<string>
#include<sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include "HttpHandler_curl.h"
#include "Logger.h"

#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include <errno.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 20000

using namespace std;

extern "C" {
#include<curl/curl.h>
}

// - - - - - - - - - - - - - - - - - - - -

enum {
    ERROR_ARGS = 1, ERROR_CURL_INIT = 2
};

enum {
    OPTION_FALSE = 0, OPTION_TRUE = 1
};

enum {
    FLAG_DEFAULT = 0
};

static const char* TAG = "HttpHandler_curl";
static bool bCurlGlobalInit = false;
int sock = -1;
string recvFail = "-1";

// - - - - - - - - - - - - - - - - - - - -
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curl_init()
{
    if(bCurlGlobalInit)
        return;

    // lubcURL 초기화
    curl_global_init (CURL_GLOBAL_ALL);
    bCurlGlobalInit = true;
}

void curl_close()
{
    if(bCurlGlobalInit) {
        curl_global_cleanup();
        bCurlGlobalInit = false;
    }
}

static bool replaceDomainToIpAddr(std::string &uri)
{
    size_t pos = -1, num = 0;

    pos = uri.find("://");
    if (pos == std::string::npos) {
        pos = 0;
        //return false;
    }
    pos += 3;

    num = uri.find(":", pos);
    if (num == std::string::npos) {
        num = uri.find("/", pos);
        if (num == std::string::npos) {
            num = uri.size();
            //return false;
        }
    }
    num -= pos;

    std::string domain = uri.substr(pos, num);
    struct hostent *host_entry = gethostbyname(domain.c_str());
    if (!host_entry)
    {
        L_DEBUG( TAG,"gethostbyname() fail!");
        return false;
    }

    //for ( idx = 0; NULL != host_entry->h_addr_list[idx]; idx++)
    //    printf( "[%d] %s\n", idx, inet_ntoa( *(struct in_addr*)host_entry->h_addr_list[idx]));

    if (NULL != host_entry->h_addr_list[0]) {
        uri.replace(pos, num, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
        L_DEBUG(TAG,"%s() : uri=%s", __FUNCTION__, uri.c_str());
        return true;
    }

    return false;
}

void socket_close() {
	close(sock);
	sock = -1;
}

/**
 Send data to the connected host
 */
bool send_data(string data) {
	//Send some data
	if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0) {
		perror("Send failed : ");
		return false;
	}
	return true;
}

/**
 Receive data from the connected host
 */
string receive() {
	char buffer[BUFF_SIZE];
	string reply = "";

	int result_size = BUFF_SIZE;

	//Receive a reply from the server
	while (1) {
				if (result_size == 0) {
					break;
				} else {
					memset(buffer, 0, BUFF_SIZE);

					if( (result_size = recv(sock, buffer, BUFF_SIZE, 0)) < 0){
						return recvFail;
					} else {
						reply.append(string(buffer).c_str());
					}
				}
	}
	//L_DEBUG(TAG, "received SUCCESS \n");
	return reply.c_str();
}

/**
 Connect to a host on a certain port number
 */
bool conn(string address, int port) {
	//create socket if it is not already created
	struct sockaddr_in server;

	struct timeval soc_timeout, recv_timeout;
	recv_timeout.tv_sec = 20;
	recv_timeout.tv_usec = 0;

	if (sock == -1) {
		// Create socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		// Recv Timeout
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
		if (sock == -1) {
			perror("Could not create socket");
		}
	} else { /* OK , nothing */
		L_DEBUG(TAG, "Socket usually \n");
	}

	//setup address structure
	if (inet_addr(address.c_str()) == -1) {
		struct hostent *he;
		struct in_addr **addr_list;

		//resolve the hostname, its not an ip address
		if ((he = gethostbyname(address.c_str())) == NULL) {
			herror("gethostbyname");
			L_ERROR(TAG, "Failed to resolve hostname\n");
			return false;
		}

		//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
		addr_list = (struct in_addr **) he->h_addr_list;

		for (int i = 0; addr_list[i] != NULL; i++) {
			server.sin_addr = *addr_list[i];
			break;
		}
	} else { //plain ip address
		server.sin_addr.s_addr = inet_addr(address.c_str());
	}

	// Non-Blocking
	int blocking_falgs = fcntl(sock, F_SETFL, O_NONBLOCK);

	fd_set fdset;

	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);

	soc_timeout.tv_sec = 10;
	soc_timeout.tv_usec = 0;

	//Connect to remote server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	connect(sock, (struct sockaddr *) &server, sizeof(server));

	if (select(sock + 1, NULL, &fdset, NULL, &soc_timeout) == 1)
	{
		int so_error;
		socklen_t len = sizeof so_error;

		getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

		// connection check
		if (so_error == 0) { // connection
	    	fcntl(sock, F_SETFL, blocking_falgs & ~O_NONBLOCK); // Blocking
	     	return true;
		} else {	// connection failed
			return false;
		}
	 }

	return false;
}

int socket_httpget(string hostname, string *response) {

	string socketConn_host;
	string send_url;
	string send_host;
	string readbuffer = "";
	string webData = "";
	vector<string> host_element, socketConn_element, readbuffer_element;
	int count = 0;

	/*
	<hostname>
	http://webepg02.firstmedia.com:80/EPG/channels.xml
	http://webepg02.firstmedia.com:80/EPG/channelmap.txt

	http://139.228.191.244/EPG/schedule/BeritaSatu.xml
	http://139.228.191.244/EPG/schedule/SCTV.xml
	*/
	count = splitStringByDelimiter(hostname, "/", host_element);

	if (count == 4){  // http: ||   || webepg02.firstmedia.com:80 || EPG || channels.xml
		splitStringByDelimiter(host_element[2], ":" , socketConn_element);
		socketConn_host = socketConn_element[0].c_str();
		send_url = host_element[3] +"/" +host_element[4];
	}
	else if (count == 5) { // http: ||   || 139.228.191.244 || EPG || schedule || BeritaSatu.xml
		socketConn_host = host_element[2].c_str();
		send_url = host_element[3] +"/" +host_element[4] +"/" +host_element[5];
	}

	//socektConn_host = "webepg02.firstmedia.com";
	//send_url = "/EPG/channels.xml";
	send_host = "GET /" +send_url +" / HTTP/1.1\r\n\r\n";

	bool result = conn(socketConn_host, 80);

	if (result == 0){
		socket_close();
		L_DEBUG(TAG, "Connetion Failed!\n");
		return -1;
	}

	//send some data
	send_data(send_host);

	readbuffer = receive();

	if (readbuffer == recvFail){
		L_DEBUG(TAG, "Received Failed!\n");
		socket_close();
		return -1;
	}

	splitStringByDelimiter(readbuffer, "\r\n\r\n", readbuffer_element);

	webData = readbuffer_element[readbuffer_element.size() - 1];
	*response = webData;
	socket_close();

	return 200;
}

long httpget(const char* url, string *response)
{
	// lubcURL 초기화
	curl_init ();

    // context객체의 생성
    CURL* ctx = curl_easy_init();
    string readBuffer="";
    std::string newUrl = url;

    replaceDomainToIpAddr(newUrl);

    long statLong;

    if(NULL == ctx) {
        DL_ERROR(TAG, "Error from cURL: Unable to initialize cURL interface\n");
        return ERROR_CURL_INIT;
    }
    //L_DEBUG(TAG, "## TEst LAST ##### : %s \n", newUrl.c_str());
    // context 객체를 설정한다.
    // 긁어올 url을 명시하고, url이 URL정보임을 알려준다.
    curl_easy_setopt(ctx, CURLOPT_URL, newUrl.c_str());

    // no progress bar:
    curl_easy_setopt(ctx, CURLOPT_NOPROGRESS, OPTION_TRUE);

    curl_easy_setopt(ctx, CURLOPT_TIMEOUT, 60); // cURL 함수를 실행하는 데 걸린 시간의 최대값입니다.

    curl_easy_setopt(ctx, CURLOPT_CONNECTTIMEOUT, 0); // 연결 시도를 기다리는 유지 초. 0은 영원히 기다릴 유지하는 것을 의미합니다.
    /*
     By default, headers are stripped from the output.
     They can be:

     - passed through a separate FILE* (CURLOPT_WRITEHEADER)

     - included in the body's output (CURLOPT_HEADER -> nonzero value)
     (here, the headers will be passed to whatever function
     processes the body, along w/ the body)

     - handled with separate callbacks (CURLOPT_HEADERFUNCTION)
     (in this case, set CURLOPT_WRITEHEADER to a
     matching struct for the function)

     */

    // body 데이터는 string으로 저장하게 한다.
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &readBuffer);

    // context 객체의 설정 종료

    // 웹페이지를 긁어온다.
    const CURLcode rc = curl_easy_perform(ctx);

    if(CURLE_OK != rc) {
        DL_ERROR(TAG, "Error from cURL: [%s(%d)] : url[%s]\n", curl_easy_strerror(rc), rc, newUrl.c_str());
        statLong = rc;
    } else {
        // HTTP 응답코드를 얻어온다.
        curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE, &statLong);
    }

    if (statLong != 200){
    		readBuffer = "";
        	curl_easy_setopt(ctx, CURLOPT_URL, url);
        	curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);
        	curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &readBuffer);
        	const CURLcode rc = curl_easy_perform(ctx);
        	if(CURLE_OK != rc) {
        	        DL_ERROR(TAG, "Error from cURL: [%s(%d)] : url[%s]\n", curl_easy_strerror(rc), rc, newUrl.c_str());
        	        statLong = rc;
        	    } else {
        	        // HTTP 응답코드를 얻어온다.
        	        curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE, &statLong);
        	    }
     }

    curl_easy_cleanup(ctx);
    *response = readBuffer;
    return statLong;
}

/*long httpget(const char* url, string *response)
{
    // lubcURL 초기화
    curl_init ();

    // context객체의 생성
    CURL* ctx = curl_easy_init();
    string readBuffer="";
    std::string newUrl = url;
    replaceDomainToIpAddr(newUrl);
    long statLong;

    if(NULL == ctx) {
        L_ERROR(TAG, "Unable to initialize cURL interface\n");
        return ERROR_CURL_INIT;
    }

    // context 객체를 설정한다.
    // 긁어올 url을 명시하고, url이 URL정보임을 알려준다.
    curl_easy_setopt(ctx, CURLOPT_URL, newUrl.c_str());

    // no progress bar:
    curl_easy_setopt(ctx, CURLOPT_NOPROGRESS, OPTION_TRUE);

    curl_easy_setopt(ctx, CURLOPT_TIMEOUT, 60);

    curl_easy_setopt(ctx, CURLOPT_CONNECTTIMEOUT, 0);

     By default, headers are stripped from the output.
     They can be:

     - passed through a separate FILE* (CURLOPT_WRITEHEADER)

     - included in the body's output (CURLOPT_HEADER -> nonzero value)
     (here, the headers will be passed to whatever function
     processes the body, along w/ the body)

     - handled with separate callbacks (CURLOPT_HEADERFUNCTION)
     (in this case, set CURLOPT_WRITEHEADER to a
     matching struct for the function)



    // body 데이터는 string으로 저장하게 한다.
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &readBuffer);

    // context 객체의 설정 종료

    // 웹페이지를 긁어온다.
    const CURLcode rc = curl_easy_perform(ctx);

    if(CURLE_OK != rc) {
        L_ERROR(TAG, "Error from cURL: %s  : url[%s]\n", curl_easy_strerror(rc), newUrl.c_str());
        statLong = rc;
    } else {
        // HTTP 응답코드를 얻어온다.
        curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE, &statLong);
    }

    // cleanup
    curl_easy_cleanup(ctx);

    *response = readBuffer;
    return statLong;
}*/

long httppost(const char* url, const char* postParams[], string *response)
{
    std::string readBuffer="";
    long statLong;
    std::string newUrl = url;
	replaceDomainToIpAddr(newUrl);

    curl_init ();

    CURL* ctx = curl_easy_init();

    if(NULL == ctx) {
        L_ERROR(TAG, "Unable to initialize cURL interface\n");
        return ERROR_CURL_INIT;
    }

    /* BEGIN: configure the handle: */

    // Target URL:
    curl_easy_setopt(ctx, CURLOPT_URL, newUrl.c_str());
    // no progress bar:
    curl_easy_setopt(ctx, CURLOPT_NOPROGRESS, OPTION_TRUE);

    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &readBuffer);

    // 사용자 정의 HTTP 헤더: create a linked list and assign
    curl_slist* responseHeaders = NULL;
    responseHeaders = curl_slist_append(responseHeaders, "length");
    responseHeaders = curl_slist_append(responseHeaders, "text/xml; charset=utf-8");
    curl_easy_setopt(ctx, CURLOPT_HTTPHEADER, responseHeaders);

    // POST Data 설정
    // notice the URL-unfriendly characters (such as "%" and "&")
    // URL에서는 '%', '&', ' '와 같은 문자를 URL encoding 시켜줘야 한다.
    // curl_escape 함수를 이용해서 인코딩할 수 있다.

    // buffer for the POST params
    std::ostringstream postBuf;

    const char** postParamsPtr = postParams;

    while (NULL != *postParamsPtr) {
        // curl_escape( {string} , 0 ): replace special characters
        // (such as space, "&", "+", "%") with HTML entities.
        // ( 0 => "use strlen to find string length" )
        // remember to call curl_free() on the strings on the way out
        char* key = curl_escape(postParamsPtr[0], FLAG_DEFAULT);
        char* val = curl_escape(postParamsPtr[1], FLAG_DEFAULT);

        L_INFO(TAG, "Setting POST param: \" %s\" => \" %s \" \n", key, val);
        postBuf << key << "=" << val << "&";

        postParamsPtr += 2;

        // the cURL lib allocated the escaped versions of the
        // param strings; we must free them here
        curl_free(key);
        curl_free(val);
    }

    postBuf << std::flush;

    // We can't really call "postBuf.str().c_str()" here, because
    // the std::string created in the middle is a temporary.  In turn,
    // the char* buf from its c_str() operation isn't guaranteed to
    // be around after the function call.
    // The solution: explicitly create the string.

    // Larger (and/or better) code would use std::string::copy() to create
    // a const char* pointer to pass to cURL, then clean it up later.
    // e.g.:
    //  const char* postData = new char*[ 1 + postBuf.tellg() ] ;
    //  postBuf.str().copy( postData , std::string::npos ) ;
    //  postData[ postBuf.tellg() ] == '\0' ;
    const std::string postData = postBuf.str();

    L_DEBUG(TAG, "post data: [%s]\n", postData.c_str());
    curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, postData.c_str());

    // do a standard HTTP POST op
    // in theory, this is automatically set for us by setting
    // CURLOPT_POSTFIELDS...
    curl_easy_setopt(ctx, CURLOPT_POST, OPTION_TRUE);

    /* END: configure the handle */
    // action!
    CURLcode rc = curl_easy_perform(ctx);

    // "curl_easy_strerror()" available in curl v7.12.x and later
    if(CURLE_OK != rc) {
        L_ERROR(TAG, "Error from cURL: %s  : url[%s]\n", curl_easy_strerror(rc), newUrl.c_str());
        statLong = rc;
    } else {
        // HTTP 응답코드를 얻어온다.
        curl_easy_getinfo(ctx, CURLINFO_HTTP_CODE, &statLong);
    }

    // cleanup
    curl_slist_free_all(responseHeaders);
    curl_easy_cleanup(ctx);

    *response = readBuffer;
    return statLong;
}

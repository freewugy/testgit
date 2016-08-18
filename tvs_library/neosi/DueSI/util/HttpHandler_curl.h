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

#ifndef _HTTP_HANDLER_CURL_H_
#define _HTTP_HANDLER_CURL_H_

#include <string>


using namespace std;

void curl_init();
void curl_close();

void socket_close();
bool conn(string address, int port);
bool send_data(string data);
string receive();
int socket_httpget(string hostname, std::string *response);
long httpget(const char* url, std::string * response);
long httppost(const char*  url, const char* postParams[], std::string * response);

#endif /* _HTTP_HANDLER_CURL_H_ */

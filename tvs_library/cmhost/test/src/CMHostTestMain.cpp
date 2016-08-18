#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <pthread.h>
#include <CMHostTest.h>
#include <utils/Log.h>

using namespace std;
using namespace android;

#define INPUT_MAXLEN 20

static pthread_t sThread = NULL;
static const char* sConsole = "# CMHostTest > ";

void * threadLoop(void *data){
	CMHostTest *testApp = (CMHostTest*)data;
	int len = 0;

	if(testApp){
		char *key = new char[INPUT_MAXLEN];

		testApp->PrintMenu();

		while(!testApp->isQuit()){
			printf("%s", sConsole);
			memset(key, 0x00, sizeof(char)*INPUT_MAXLEN);
			fgets(key, INPUT_MAXLEN, stdin);

			len = strlen(key)-1;

			if(!key || len ==0)
				continue;
			CMLOGD("Input key : %s\n", key);
			testApp->RunCMD(key);
		}

		if(key){
			delete[] key;
			key= NULL;
		}
	}

	return NULL;
}

int main(int argc, char *argv[]){
	CMHostTest *app = new CMHostTest();
	int status;

	CMLOGD("\n\n Start CMHostTest App...\n\n");
	pthread_create(&sThread, NULL, &threadLoop, app);
	pthread_join(sThread, (void**)&status);
	
	if(app){
		delete app;
		app = NULL;
	}

	CMLOGD("\n\nComplete CMHostTest App....\n\n");
	return 0;
}

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <ctype.h>
#include <locale.h>
#include <limits.h>

#include "SIManager.h"
#include "NeoSIHandler.h"
#include "util/TimeConvertor.h"
#include "util/StringUtil.h"
#include "util/Timer.h"

NeoSIHandler* pHandler = NULL;

void NeoSIStart(int type)
{
    printf("%s === Called === type [%d] ===\n", __FUNCTION__, type);

    if(pHandler == NULL)
        pHandler = new NeoSIHandler();

    pHandler->start(type);
}

void NeoSIStop()
{
    if(pHandler != NULL) {
        pHandler->stop();
    } else {
        printf("NOT STARTED\n");
    }

}

void printMainMenu()
{
    printf("To quit, enter              : 'q'\n");
    printf("Linknet SI Start - DummyEPG : 'a' or 'A' \n");
    printf("Linknet SI Start - WebSI    : 'z' or 'Z' \n");
    printf("Linknet SI Stop             : 's' or 'S' \n");

}

void printSecondMenu()
{
    printf("To quit, enter 						: 'q'\n");
    printf("get channel list only   			: 'c'\n");
    printf("get channel list with program       : 'i'\n");
    printf("get program list					: '1' \n");
    printf("get current program 					: '2' \n");
    printf("get program by index(0~10)				: '3' \n");
    printf("get program by time(current ~ 8hour)			: '4' \n");
    printf("get program by name					: '5' \n");
    printf("go back main menu....					: 'b' or 'B' \n");
}

void printEnterValue(string str)
{
    printf("To quit, enter 					: 'q'\n");
    printf("#################################\n");
    printf("Enter %s value		: \n", str.c_str());
    printf("#################################\n");
    printf("go back main menu....				: 'b' or 'B' \n");

}

int main(int argc, char* argv[])
{
	int menulevel = 0;
    while (1) {

        switch (menulevel) {
            case 0:
            printMainMenu();
                break;

            case 1:
            printSecondMenu();
                break;

            default:
                break;
        }

        if(menulevel == 2) {

        } else if(menulevel == 3) {
            menulevel = 1;

        } else {
            int ch = getchar();
            switch (ch) {
                case 'm':
                case 'M': {
                    menulevel = 0;
                    break;
                }
                case 'a':
                case 'A': {
                    NeoSIStart(1);
                    menulevel = 1;
                    break;
                }
                case 'b':
                case 'B': {
                    if(menulevel != 0)
                        menulevel--;
                    ;
                    break;
                }
                case 'c':
                case 'C': {
                    if(pHandler != NULL) {
                        pHandler->printchannellist(false);
                    }
                    menulevel = 1;
                    break;
                }
                case 'l':
                case 'L': {
                    break;
                }

                case 'g':
                case 'G': {
                    break;

                }
                case 's':
                case 'S': {
                    NeoSIStop();
                    menulevel = 0;
                    break;
                }
                case 'd':
                case 'D':
                menulevel = 0;
                    break;

                case '1':
                case '2':
                case '3':
                case '4':
                menulevel = 2;
                    break;

                case '5':
                menulevel = 2;
                    break;

                case 'u':
                    break;
                    
                case 'v':
                    break;

                case 't':
                case 'T':
                    break;
                case 'p':
                case 'P':
                if(pHandler != NULL) {
                    pHandler->printProductInfo();
                }
                menulevel = 1;
                    break;

                case 'z':
                case 'Z': {
                    NeoSIStart(2);
                    break;
                }
                case 'i':
                case 'I':
                if(pHandler != NULL) {
                    pHandler->printchannellist(true);
                }
                menulevel = 1;
                    break;
                case 'q':
                return 0;
                    break;

            }
        }

    }

    return 0;
}


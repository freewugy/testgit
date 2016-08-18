/*
 * log.c
 *
 *  Created on: Jan 26, 2015
 *      Author: jhseo
 */

#include <stdarg.h>
#include <stdio.h>

#if defined(DEBUG)
//#define MAX_BUFF_SIZE	1024
//
//char log_buff[MAX_BUFF_SIZE];
//
//void loge(char* logFmt, ...) {
//	va_list argList;
//
//	va_start(argList, logFmt);
//	vsprintf(log_buff, logFmt, argList);
//
//	printf(log_buff);
//    va_end(argList);
//}
//
//void logw(char* logFmt, ...) {
//	va_list argList;
//
//    va_start(argList, logFmt);
//	vsprintf(log_buff, logFmt, argList);
//
//	printf(log_buff);
//    va_end(argList);
//}
//
//void logi(char* logFmt, ...) {
//	va_list argList;
//
//    va_start(argList, logFmt);
//	vsprintf(log_buff, logFmt, argList);
//
//	printf(log_buff);
//    va_end(argList);
//}
//
//void logd(char* logFmt, ...) {
//	va_list argList;
//
//    va_start(argList, logFmt);
//	vsprintf(log_buff, logFmt, argList);
//
//	printf(log_buff);
//    va_end(argList);
//}
#else
void loge(char* log) {

}

void logw(char* log) {

}

void logi(char* log) {

}

void logd(char* log) {

}
#endif

#ifndef __KERNEL_LOG_H__
#define __KERNEL_LOG_H__

static int startKernelLog();
static int openKernelLog(char *path);
static void closeKernelLog();
static int writeKernelLog(char *buf, int len);
static int rotateKernelLog();
static bool isRunning();
#endif //__KERNEL_LOG_H__

#ifndef __NUID_H_
#define __NUID_H_

#define SEC_NUID_LENGTH_32  32
#define SEC_NUID_LENGTH     64

int secGetNuid(unsigned char* pxNuid);
int secGetNuid64(unsigned char* pxNuid);

#endif

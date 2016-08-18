#ifndef __TVI_TYPES_H__
#define __TVI_TYPES_H__

typedef char 				TVIInt8;
typedef short				TVIInt16;
typedef int					TVIInt32;
typedef long long 			TVIInt64;

typedef unsigned char 		TVIUint8;
typedef unsigned short		TVIUint16;
typedef unsigned int		TVIUint32;
typedef unsigned long long	TVIUint64;

typedef TVIUint8			TVIByte;

#define TVIINT8_MIN			(-128)
#define TVIINT8_MAX			(127)
#define TVIINT16_MIN		(-32768)
#define TVIINT16_MAX		(32767)
#define TVIINT32_MIN		(-TVIINT32_MAX-1)
#define TVIINT32_MAX		2147483647

#define TVITSTUint8_MAX		(255)
#define TVITSTUint16_MAX		(65535)
#define TVITSTUint32_MAX		(4294967295U)

typedef TVIInt32			TVIBool;
#define TVI_TRUE			((TVIBool)(1))
#define TVI_FALSE			((TVIBool)(0))

#endif /*__TVI_TYPES_H__*/

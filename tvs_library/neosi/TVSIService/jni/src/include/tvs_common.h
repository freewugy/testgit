/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2011-08-29 16:22:25 +0900 (2011-08-29, 월) $
 * $LastChangedRevision: 210 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef TVS_COMMON_H_
#define TVS_COMMON_H_

typedef unsigned short TUint16;
typedef unsigned int TUint32;
typedef unsigned long long TUint64;
typedef short TInt16;
typedef int TInt32;
typedef long long TInt64;

//typedef bool TBool;
typedef char TChar;
typedef unsigned char TByte;

typedef float TFloat;
typedef double TDouble;

#include <cstddef>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <dlfcn.h>

typedef std::string String;

#endif /* TVS_COMMON_H_ */

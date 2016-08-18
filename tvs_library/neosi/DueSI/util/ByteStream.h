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

#ifndef __BYTE_STREAM_H__
#define __BYTE_STREAM_H__

#include <byteswap.h>

#define r16(p)		bswap_16(*(const uint16_t * const)p)
#define r32(p)		bswap_32(*(const uint32_t * const)p)
#define r64(p)		bswap_64(*(const uint64_t * const)p)
#define w16(p,v)	do { *(uint16_t * const)(p) = bswap_16((const uint16_t)v) } while (0)
#define w32(p,v)	do { *(uint32_t * const)(p) = bswap_32((const uint32_t)v) } while (0)
#define w64(p,v)	do { *(uint64_t * const)(p) = bswap_64((const uint64_t)v) } while (0)

#define DVB_LENGTH(p)   (r16(p) & 0x0fff)
#define DVB_PID(p)  (r16(p) & 0x1fff)

#define TableIDExt(p) (((uint16_t)p[3] << 8) |((uint16_t)p[4]))
#define Version(p) ((p[5]>>1) & 0x1F)

#define EtmID(p) (((uint32_t)p[9] << 24) |((uint32_t)p[10] << 16) |((uint32_t)p[11] << 8) |((uint32_t)p[12]))

#define DVBEITTSID(p) (((uint16_t)p[8] << 8) |((uint16_t)p[9]))
#define SectionLength(p) ((((p[1]&0x0F) << 8) | p[2]) + 3)

#endif /* __BYTE_STREAM_H__ */

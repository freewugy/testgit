/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2015. 11. 11. $
 * $LastChangedRevision: 80 $
 * Description:
 * Note:
 *****************************************************************************/

#define TVS_LOG_ENABLED
#define LOG_TAG "flash_ts"

#include "flash_ts.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/*
 * Set fts standby mode
 * input :
 *           opt : OP_SET is write fts key, OP_GET is read fts key
 *           sb_value : fts val
 * output :
 *           opt == OP_SET,  return : 0 is success, -1 or other is faile
 *           opt == OP_GET,  return : 1 is Standby mode, 0 is Active mode, other is faile
 */
int standby_fts_ctrl(int opt, char * sb_value)
{
	struct flash_ts_io_req req;
	int fd, op = opt, res = -1;

	fd = open(FTS_DEVICE, O_RDWR);
	if (fd < 0)
		return -1;

	memset(&req, 0, sizeof(req));
	strlcpy(req.key, STANDBY_KEY, sizeof(req.key));
	if(sb_value != NULL)
		strlcpy(req.val, sb_value, sizeof(req.val));

	if (ioctl(fd, op == OP_SET ? FLASH_TS_IO_SET : FLASH_TS_IO_GET, &req))
		res = -2;
	else
		res = 0;

	close(fd);

	if(res == 0 && OP_GET == op)
	{
		if(strncmp(req.val, BOOT_STANDBY_ON, sizeof(req.val)) == 0)
			res = 1;
		else
			res = 0;
	}

	return res;
}

/*
 * Get fts standby mode
 * return :  1 is Standby status, 0 is Active status, other value is error
 */
int get_fts_standby(void)
{
	int sb_mode = 0;

	sb_mode = standby_fts_ctrl(OP_GET, NULL);

	return sb_mode;
}

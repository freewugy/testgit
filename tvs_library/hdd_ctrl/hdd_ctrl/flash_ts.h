/*
 * Flash-based transactional key-value store user-space API
 *
 * Copyright (C) 2010 Google, Inc.
 *
 */
#ifndef _LINUX_FLASH_TS_H_
#define _LINUX_FLASH_TS_H_

#include <asm/ioctl.h>
#include <asm/types.h>

#define FLASH_TS_MAX_KEY_SIZE 64
#define FLASH_TS_MAX_VAL_SIZE 2048

struct flash_ts_io_req {
  char key[FLASH_TS_MAX_KEY_SIZE];
  char val[FLASH_TS_MAX_VAL_SIZE];
};

#define FLASH_TS_IO_MAGIC    0xFE
#define FLASH_TS_IO_SET      _IOW(FLASH_TS_IO_MAGIC, 0, struct flash_ts_io_req)
#define FLASH_TS_IO_GET      _IOWR(FLASH_TS_IO_MAGIC, 1, struct flash_ts_io_req)

#define FTS_DEVICE           "/dev/fts"

// fts
#define STANDBY_KEY "bootloader.standby"
#define BOOT_STANDBY_ON "on"
#define BOOT_STANDBY_OFF "off"

enum
{
	OP_NONE, OP_SET, OP_GET
};

int standby_fts_ctrl(int opt, char * sb_value);
int get_fts_standby(void);

#endif  /* _LINUX_FLASH_TS_H_ */

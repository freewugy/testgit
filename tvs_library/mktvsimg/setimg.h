#ifndef __SET_IMG_H_
#define __SET_IMG_H_

int set_version(char* file, char* version);
int set_forced(char* file, int forced);

int tvsimg_file_read(const char* path, uint8 **buf, uint32 *size);
int tvsimg_file_write(const char* path, uint8 *buf, uint32 size);

#endif

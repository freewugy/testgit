#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

#if 0
int read_tvsimg_hdr(char* file, TVS_IMG *tvsimg)
{
    FILE* fd;
    int n = 0;

    fd = fopen(file, "rb");
    if (!fd)
    {
        printf("open fail %s\n", file);
        return -1;
    }

    n = fread(tvsimg, 1, sizeof(TVS_IMG), fd);
    if(n != sizeof(TVS_IMG))
    {
        fprintf(stderr, "%s corrupted! \n", file);
        return -2;
    }

    fclose(fd);

    return 0;
}

int write_tvsimg_hdr(char* file, TVS_IMG *tvsimg)
{
    FILE* fd;
    int n = 0;

    fd = fopen(file, "wb");
    if (!fd)
    {
        printf("open fail %s\n", file);
        return -1;
    }

    n = fwrite(tvsimg, 1, sizeof(TVS_IMG), fd);
    if(n != sizeof(TVS_IMG))
    {
        fprintf(stderr, "%s corrupted! \n", file);
        return -2;
    }

    fclose(fd);

    return 0;
}
#endif

int set_version(char* file, char* ver_str)
{
    TVS_IMG *tvsimg;
    char *ver, *major_s, *minor_s, *patch_s;
    uint32 version = 0, major, minor, patch, size;
    uint8 *buf;

    LOGI("file %s, version %s \n", file, ver_str);

    ver = ver_str;
    major_s = strtok(ver, ".");
    minor_s = strtok(NULL, ".");
    patch_s = strtok(NULL, ".");

    major = strtoul(major_s, NULL, 10);
    minor = strtoul(minor_s, NULL, 10);
    patch = strtoul(patch_s, NULL, 10);

    version = (major & 0xFF) << 16 | (minor & 0xFF) << 8 | (patch & 0xFF);
    LOGI("version 0x%08x \n", version);

    tvsimg_file_read(file, &buf, &size);

    tvsimg = (TVS_IMG*)buf;

    tvsimg->ver = version;

    tvsimg_file_write(file, buf, size);

    return 0;
}

int set_forced(char* file, int forced)
{
    TVS_IMG *tvsimg;
    uint32 flag, n=0, size;
    uint8 *buf;
    int ret = 0;
    FILE* fd;

    //LOGI("file %s, forced %d \n", file, forced);

    //LOGI("file size: %lld(0x%llx) \n", fs.st_size, fs.st_size);
    tvsimg_file_read(file, &buf, &size);
    LOGI("buf %p \n", buf);

    tvsimg = (TVS_IMG*)buf;

    flag = tvsimg->flag;

    flag = SET_FORCED_UP(forced, flag);

    tvsimg->flag = flag;

    tvsimg_file_write(file, buf, size);

    return 0;
}

int tvsimg_file_read(const char* path, uint8 **buf, uint32 *size)
{
    uint *file_buf;
    int ret, n;
    struct stat fs;
    FILE *fd;

    stat(path, &fs);

    *size = fs.st_size;

    file_buf = malloc(fs.st_size);

    LOGI("file_buf %p \n", file_buf);

    if(NULL == file_buf)
    {
        printf("malloc failed! \n");
        return -1;
    }

    fd = fopen(path, "rb");
    if (!fd)
    {
        printf("open fail %s\n", path);
        return -1;
    }

    n = fread(file_buf, 1, fs.st_size, fd);
    if(n != fs.st_size)
    {
        fprintf(stderr, "%s corrupted! \n", path);
        return -2;
    }
    fclose(fd);

    *buf = file_buf;

    return 0;
}

int tvsimg_file_write(const char* path, uint8 *buf, uint32 size)
{
    int ret, n;
    FILE *fd;

    ret = remove(path);

    //printf("ret %d \n", ret);

    if(ret)
    {
        return -1;
    }

    fd = fopen(path, "wb");
    if (!fd)
    {
        printf("open fail %s\n", path);
        return -1;
    }

    n = fwrite(buf, 1, size, fd);
    if(n != size)
    {
        fprintf(stderr, "%s corrupted! \n", path);
        return -2;
    }
    fclose(fd);

    return 0;
}

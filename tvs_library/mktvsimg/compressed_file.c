
#include <stdio.h>
#include <errno.h>

#include "common.h"

int get_gz_file_info(char *file_path, uint32 *crc32, uint32 *length)
{
    uint32 uncompressed_size;
    long pos;
    FILE *f;
    int i;

    printf("get_gz_file_info %s \n", file_path);

    f = fopen(file_path, "rb");

    if(NULL == f)
    {
        printf("open failed: %s \n", file_path);
        return -1;
    }

    pos = fseek(f, -8L, SEEK_END);
    if(pos < 0)
    {
        printf("error: %d \n", errno);
        return -2;
    }

    if(4 != fread(crc32, 1, 4, f))
    {
        printf("read crc32 error: %d \n", errno);
        return -3;
    }

    if(4 != fread(length, 1, 4, f))
    {
        printf("read length error: %d \n", errno);
        return -4;
    }

    printf("uncompressed size %d crc 0x%08x\n", *length, *crc32);

    return 0;
}

int compress_file(char* src_path)
{
    FILE *f;

    //LOGI("src_path %s \n", src_path);
    f = fopen(src_path, "rb");
    if (f == NULL) {
        printf("open error %s \n", src_path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    unsigned int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *compressed;
    //LOGI("len %d \n", len);
    compressed = malloc(len);
    if(NULL == compressed)
    {
        printf("fail malloc \n");
        fclose(f);
        return -1;
    }
    int re = def(f, compressed, 1, &len);
    //LOGI("ret %d, len %d\n", re, len);

    f = fopen("comp.gz", "wb");
    int wlen = fwrite(compressed, 1, len, f);
    //LOGI("wlen %d \n", wlen);
    fclose(f);

    return 0;
}



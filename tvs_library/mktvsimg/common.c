#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

void print_hash(uint8 *hash)
{
    int i;

    printf("     hash      : ");
    for(i=0; i<SHA256_DIGEST_LENGTH; i++)
    {
        if(0 != i)
            if(!(i%16)) printf("\n                 ");
        printf("%02x ", hash[i]);
    }
    printf("\n");
}

uint32 get_ver_from_string(const char* ver)
{
    char* ver_str = (char*)ver;
    char *major_s, *minor_s, *patch_s;
    uint32 version = 0, major, minor, patch;
    FILE *file;

    LOGI("get_ver_from_string: %s \n", ver);

    major_s = strtok(ver_str, ".");
    minor_s = strtok(NULL, ".");
    patch_s = strtok(NULL, ".");

    major = strtoul(major_s, NULL, 10);
    minor = strtoul(minor_s, NULL, 10);
    patch = strtoul(patch_s, NULL, 10);

    version = (major & 0xFF) << 16 | (minor & 0xFF) << 8 | (patch & 0xFF);

    LOGI("get_ver_from_string: 0x%08x \n", version);
    return version;
}

uint32 get_version(const char* basedir, const char* fn)
{
    char line[256];
    char *major_s, *minor_s, *patch_s;
    uint32 version = 0, major, minor, patch;
    FILE *file;

    char path[PATH_MAX];

    sprintf(path, "%s/%s", basedir, fn);

    file = fopen(path, "r");
    if (!file)
    {
        printf("open fail %s\n", path);
        return 0;
    }

    if(fgets(line, 256, file) != NULL)
    {
        //LOGI("line: %s \n", line);
        version = get_ver_from_string(line);
#if 0
        major_s = strtok(line, ".");
        minor_s = strtok(NULL, ".");
        patch_s = strtok(NULL, ".");

        major = strtoul(major_s, NULL, 10);
        minor = strtoul(minor_s, NULL, 10);
        patch = strtoul(patch_s, NULL, 10);

        version = (major & 0xFF) << 16 | (minor & 0xFF) << 8 | (patch & 0xFF);
#endif
    }

    return version;
}

unsigned int get_page_aligned_size(unsigned int size, int pagesize)
{
    unsigned int pagemask = pagesize - 1;
    unsigned int count = 0;

    if((size & pagemask) == 0) {
        count = 0;
    }
    else
    {
        count = pagesize - (size & pagemask);
    }

    return size + count;
}


/* tools/mkbootimg/mkbootimg.c
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <zlib.h>
#include <libgen.h>

#include "ini.h"
#include "common.h"
#include "compressed_file.h"
#include "gz.h"
#include "sign_file.h"
#include "setimg.h"

#define TRUE    1
#define FALSE   0

void set_verify_value(uint8* data, int size, void* verify, int integrity_method);

int usage(void)
{
    fprintf(stderr,
            "usage: mktvsimg\n"
            "       --config <filename> | --file <filename>\n"
            "       [ --base <base working directory> ]\n"
            "       [ --output <filename> ]\n"
            "       [ --setver <version string> ]\n"
            "       [ --setforced <1 or 0> ]\n"
            "       [ --extract <part name> ]\n");
    return 1;
}

/*
 * 파일이 downloader 이미지인 경우 헤더가 포함됨.
 */
static uint32 load_file(const char *fn, int idx, MKTVSIMG *mktvsimg)
{
    TVS_IMG *hdr = &mktvsimg->tvs_img;
    img_info *imginfo = &(hdr->img[idx]);
    uint8 *temp, *file_buf, buf[4];
    FILE *fd;
    int  is_compress;
    uint32 img_sz, file_buf_size;
    uint32 len, version;
    char *part_name = (char*)imginfo->flash_part_name;

    //LOGI("-->[%d] load_file %s \n\n", idx, fn);

    is_compress = GET_COMPRESS(imginfo->flag);

    if(mktvsimg->integrity)
    {
        imginfo->flag = SET_INTEGRITY(mktvsimg->integrity == ALL_SHA256, imginfo->flag);
    }

    fd = fopen(fn, "rb");
    if(NULL == fd)
    {
        printf("ERROR: open %s failed \n", fn);
        //LOGI("<-- [%d] load_file %s \n", idx, fn);
        return 0;
    }

    if(mktvsimg->version_from_img[idx])
    {
        // signed image의 경우 이미지 자체 버전정보를 가져온다.
        if(!strcmp(part_name, "boot")   || !strcmp(part_name, "recovery") || 
           !strcmp(part_name, "recovery_backup") || !strcmp(part_name, "diagnostics") || 
           !strcmp(part_name, "splash") || !strcmp(part_name, "nbls2") || 
           !strcmp(part_name, "nbls3")  || !strcmp(part_name, "nbls3_backup"))
        {
            if(4 != fread(buf, 1, 4, fd))
            {
                goto oops;
            }
            len = *(uint32*)buf;
            len = ntohl(len);
            fseek(fd, len, SEEK_CUR);
            if(4 != fread(buf, 1, 4, fd))
            {
                fprintf(stderr, "ERROR: version info error!\n");
                goto oops;
            }
            version = *(uint32*)(buf);
            imginfo->ver = 0x00FFFFFF & ntohl(version);
            LOGI("%s : ver 0x%08x \n", part_name, imginfo->ver);
        }
        else if(!strcmp(part_name, "system"))
        {
            fseek(fd, -4, SEEK_END);
            if(4 != fread(buf, 1, 4, fd))
            {
                fprintf(stderr, "ERROR: version info error!\n");
                goto oops;
            }
            version = *(uint32*)(buf);
            imginfo->ver = 0x00FFFFFF & ntohl(version);
            LOGI("%s : ver 0x%08x \n", part_name, version);
        }
    } 
    else
    {
        LOGI("%s : ver 0x%08x \n", part_name, imginfo->ver);
    }

    // 이미지 파일 사이즈를 얻어옴.
    fseek(fd, 0, SEEK_END);
    img_sz = (uint32)ftell(fd);
    //if(img_sz < 0) goto oops;

    // file pointer를 처음으로 다시 돌림.
    if(fseek(fd, 0, SEEK_SET) != 0) 
    {
        fprintf(stderr, "SEEK error \n");
        goto oops;
    }

    file_buf = mktvsimg->file_buf[idx] = malloc(img_sz+sizeof(DLDR));
    if(NULL == file_buf) 
    {
        fprintf(stderr, "malloc error (%d bytes)\n", img_sz+sizeof(DLDR));
        goto oops;
    }

    /////////////////////////////////////////////////////////////////////////////
    // downloader 또는 downloader backup 파티션 이미지는 자체 헤더를 포함해야 한다.
    /////////////////////////////////////////////////////////////////////////////
    if(!strcmp("recovery", part_name) || !strcmp("recovery_backup", part_name))
    {
        DLDR *dldr   = (DLDR*)file_buf;
        dldr->magic  = MAGIC_DLDR;
        dldr->ver    = imginfo->ver;
        dldr->length = img_sz;

        temp = file_buf + sizeof(DLDR);
        if(fread(temp, 1, img_sz, fd) != img_sz) goto oops;

        set_verify_value(temp, img_sz, &(dldr->crc32), CRC_32);
        memcpy(imginfo->id, &(dldr->crc32), 4);
        //memcpy(data, dldr, sizeof(DLDR));
        file_buf_size = img_sz + sizeof(DLDR);
    }
    else
    {
        if(is_compress)
        {
            uint32 src_file_size = img_sz;
            printf("compressing %s \n", fn);
            // img_sz : in -> 원본파일 크기, out -> 압축파일 크기
            def(fd, file_buf, 1, &img_sz);
            printf("\033[F\033[J");
            printf("\033[F\033[J");

            // 압축파일은 파일 Footer에 CRC32와 length를 포함하고 있으므로 
            uint32 *crc32 = (uint32*)imginfo->id;
            memcpy(crc32, file_buf + img_sz - 8, 4);
            //LOGI("crc (gz footer): %08x \n", *crc32);

            uint32 *length = (uint32*)(file_buf + img_sz - 4);
            //LOGI("len (gz footer): %08x \n", *length);

            imginfo->uncomp_size = src_file_size;
            imginfo->size = img_sz;
        }
        else
        {
            if(fread(file_buf, 1, img_sz, fd) != img_sz) goto oops;
            set_verify_value(file_buf, img_sz, 
                imginfo->id, GET_INTEGRITY(imginfo->flag));
            //LOGI("file_buf %p \n", file_buf);
        }
        file_buf_size = img_sz;
    }
    imginfo->size = img_sz;

    fclose(fd);

    //LOGI("<-- [%d] load_file %s \n", idx, fn);
    return file_buf_size;

oops:
    fprintf(stderr, "oops! \n");
    fclose(fd);
    return 0;
}

static uint8 padding[16384] = { 0, };

int write_padding(int fd, unsigned pagesize, unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    ssize_t count;

    if((itemsize & pagemask) == 0) {
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(write(fd, padding, count) != count) {
        return -1;
    } else {
        return 0;
    }
}

/* return 1 on succes */
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    MKTVSIMG *mktvsimg = (MKTVSIMG *)user;
    TVS_IMG *hdr = &mktvsimg->tvs_img;
    img_info *img;
    static char prev_section[32] = {0};
    static int idx=-1, disabled_section = 0;

    //LOGI("section %s \n", section);

    if(!strcmp(section, "info"))
    {
        //LOGI("name : %s \n", name);
        if(!strcmp(name, "pagesize"))
            hdr->page_size = strtoul(value, NULL, 10);
        else if(!strcmp(name, "filename"))
        {
            sprintf(mktvsimg->update_fn, "%s/%s", mktvsimg->basedir, value);
        }
        else if(!strcmp(name, "product"))
            strcpy((char*)hdr->prod_name, value);
        else if(!strcmp(name, "version"))
        {
            if(('0' == value[0] && 'x' == value[1])) 
            {
                LOGI("version from hex %s\n", value);
                // 0x00010203
                hdr->ver = (uint32)strtoul(value, 0, 16);
                LOGI("version from hex 0x%x\n", hdr->ver);
            }
            else if(isdigit(value[0]))
            {
                LOGI("version from string \n");
                // 1.2.4
                hdr->ver = get_ver_from_string(value);
            }
            else
            {
                LOGI("version from file\n");
                // from file
                hdr->ver = get_version(mktvsimg->basedir, value);
            }
        }
        else if(!strcmp(name, "desc"))
        {
            strcpy((char*)hdr->extra_info, value);
        }
        else if(!strcmp(name, "emmc_resize"))
        {
            //LOGI("gpt_resize %s \n", value);
            hdr->flag = SET_EMMC_RESIZE(!strcmp("true", value), hdr->flag);
        }
        else if(!strcmp(name, "spi_resize"))
        {
            //LOGI("spi_resize %s \n", value);
            hdr->flag = SET_SPI_RESIZE(!strcmp("true", value), hdr->flag);
        }
        else if(!strcmp(name, "integrity"))
        {
            mktvsimg->integrity = !strcmp("sha256", value)?ALL_SHA256:ALL_CRC32;
        }
        else if(!strcmp(name, "forced"))
        {
            hdr->flag = SET_FORCED_UP(!strcmp("true", value), hdr->flag);
        }
        else if(!strcmp(name, "key_path"))
        {
            sprintf(mktvsimg->key_path, "%s/%s", mktvsimg->basedir, value);
        }
        else if(!strcmp(name, "key"))
        {
            strcpy(mktvsimg->key_prefix, value);
        }
        return 1;
    }
    else
    {
        if ('*' == section[0] || '#' == section[0])
        {
            //LOGI("[%s] disabled \n", section);
            disabled_section = TRUE;
        }
        else
        {
            disabled_section = FALSE;
            if(strcmp(prev_section, section))
            {
                idx++;
                hdr->img_num = idx+1;
                strcpy(prev_section, section);
            }
        }
    }

    if(disabled_section)
    {
        //LOGI("%16s : %s \n", name, value);
        return 1;
    }

    img = &hdr->img[idx];

    LOGI("[%d] section: %s, name: %s, value: %s \n", idx, section, name, value);
    if(!strcmp(name, "filename"))
    {
        sprintf(mktvsimg->filename[idx], "%s/%s", mktvsimg->basedir, value);
        //LOGI("idx %d value %s mktvsimg->filename[%d] %s\n", idx, value, idx, mktvsimg->filename[idx]);
    }
    else if(!strcmp(name, "src_file"))
    {
        // 압축파일 소스
        sprintf(mktvsimg->uncomp_filename[idx] , "%s/%s", mktvsimg->basedir, value);
    }
    else if(!strcmp(name, "compressed"))
    {
        //LOGI("idx %d value %s flag 0x%08x\n", idx, value, img->flag);
        img->flag = SET_COMPRESSED(!strcmp(value, "true"), img->flag);
        //LOGI("idx %d value %s flag 0x%08x\n", idx, value, img->flag);
    }
    else if(!strcmp(name, "integrity"))
    {
        img->flag = SET_INTEGRITY(!strcmp(value, "sha256"), img->flag);
    }
    else if(!strcmp(name, "flash_part_name"))
    {
        // max length: FLASH_PART_NAME_MAX - 2
        strcpy((char*)img->flash_part_name, value);
    }
    else if(!strcmp(name, "flash_part_offset"))
    {
        off64_t offset = strtoull(value, 0, 16);
        //LOGI("[%d] flash_part_offset %lx \n", idx, offset);
        img->flash_part_offset = offset;
    }
    else if(!strcmp(name, "flash_part_type"))
    {
        img->flag = SET_FLASHTYPE(!strcmp(value, "emmc"), img->flag);
    }
    else if(!strcmp(name, "version"))
    {
        if(('0' == value[0] && 'x' == value[1])) 
        {
            img->ver = (uint32)strtoul(value, 0, 16);
        }
        else if(isdigit(value[0]))
        {
            img->ver = get_ver_from_string(value);
            LOGI("img->ver 0x%08x \n", hdr->ver);
        }
        else if(!strcmp("img", value))
        {
            mktvsimg->version_from_img[idx] = 1;
        }
        else
        {
            img->ver = get_version(mktvsimg->basedir, value);
        }
    }
    else if(!strcmp(name, "forced"))
    {
        img->flag = SET_FORCED_UP(!strcmp("true", value), img->flag);
    }
    else 
    {
        printf("unknown name (%s) \n", name);
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

int print_file_info(char *update_fn)
{
    MKTVSIMG mktvsimg;
    TVS_IMG tvsimg;
    int read = 0;
    FILE* fd;

    memset(&mktvsimg, 0, sizeof(MKTVSIMG));
    memset(&tvsimg, 0, sizeof(TVS_IMG));

    if(!update_fn)
    {
        fprintf(stderr, "update_fn is NULL \n");
        return -1;
    }

    fd = fopen(update_fn, "rb");
    if (!fd)
    {
        printf("open fail %s\n", update_fn);
        return -1;
    }

    read = fread(&tvsimg, 1, sizeof(TVS_IMG), fd);
    if(read != sizeof(TVS_IMG))
    {
        fprintf(stderr, "%s corrupted! \n", update_fn);
        return -2;
    }

    memcpy(&mktvsimg.tvs_img, &tvsimg, sizeof(TVS_IMG));

    print_img_info(&mktvsimg);

    fclose(fd);
    return 0;
}

void get_sha(uint8* data, int size, uint8 *hash)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, size);
    SHA256_Final(hash, &ctx);
}

//unsigned long crc32(unsigned long crc, const uint8* buf, uint32 len);

void set_crc32(uint8* data, int size, void *_crc32)
{
    unsigned long *p = (unsigned long *)_crc32;
    unsigned long crc;

    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, data, size);
 
    *p = crc;
    //LOGI("set_crc32(crc32 0x%08lx)\n", crc);
}


/*
 * 계산된 verify value는 verify pointer가 가르키는 곳에 저장됨
 */
void set_verify_value(uint8* data, int size, void* verify, int integrity_method)
{
    if(SHA_256 == integrity_method)
        get_sha(data, size, verify);
    else
        set_crc32(data, size, verify);
}

#define CHECK_FILE_NAME_OPT(x)         do {  if(!x) \
                                            { \
                                                fprintf(stderr, "--file option needed! \n"); \
                                                return -1; \
                                            } \
                                      } while(0)

int main(int argc, char **argv)
{
    MKTVSIMG mktvsimg;
    TVS_IMG *hdr;
    img_info *img;

    int error;
    char *config_fn = 0, *update_fn = 0, src_path[256];
    char update_fn_tmp[PATH_MAX], path_pem[PATH_MAX], path_pk8[PATH_MAX], path_x509[PATH_MAX];
    char version[32], extract_part[128];
    int i, fd, opt_prt_hdr = 0, forced=0;
    int opt_setver = 0, opt_setforced = 0, opt_file_name = 0, opt_extract = 0;
    uint32 file_write_size;
    SHA_CTX ctx;

    print_mktvsimg_info();

    argc--;
    argv++;

    memset(&mktvsimg, 0, sizeof(MKTVSIMG));

    hdr = &(mktvsimg.tvs_img);
    for(i=0; i<MAX_FILE_NUM; i++)
    {
        img = &(hdr->img[i]);

        // flash_part_offset must be initialized to -1
        img->flash_part_offset = -1;
    }

    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        if(argc < 2) {
            return usage();
        }
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            return usage();
        }
        else if(!strcmp(arg, "--output") || !strcmp(arg, "-o")) {
            strcpy(mktvsimg.update_fn, val);
#if 0
        } else if(!strcmp(arg, "--info")) {
            opt_prt_hdr = 1;
            update_fn = val;
#endif
        } else if(!strcmp(arg, "--file")) {
            opt_file_name = 1;
            update_fn = val;
        } else if(!strcmp(arg, "--config")) {
            config_fn = val;
        } else if(!strcmp(arg, "--base")) {
            strcpy(mktvsimg.basedir, val);
        } else if(!strcmp(arg, "--comp")) {
            strcpy(src_path, val);
        } else if(!strcmp(arg, "--setver")) {
            opt_setver = 1;
            strcpy(version, val);
        } else if(!strcmp(arg, "--setforced")) {
            opt_setforced = 1;
            forced = strtoul(val, 0, 10);
        } else if(!strcmp(arg, "--extract")) {
            LOGI("--extract \n");
            opt_extract = 1;
            strcpy(extract_part, val);
        } else {
            printf("arg : %s undefined.\n", arg);
            return usage();
        }
    }

    if(opt_setver)
    {
        CHECK_FILE_NAME_OPT(opt_file_name);

        set_version(update_fn, version);
        print_file_info(update_fn);
        return 0;
    }

    if(opt_setforced)
    {
        CHECK_FILE_NAME_OPT(opt_file_name);

        set_forced(update_fn, forced);
        print_file_info(update_fn);
        return 0;
    }

    if(opt_extract)
    {
        LOGI("opt_extract \n");
        CHECK_FILE_NAME_OPT(opt_file_name);
        extract_partition(extract_part, update_fn);
        return 0;
    }

    if(opt_file_name)
    {
        return print_file_info(update_fn);
    }

    if(config_fn == 0) {
        fprintf(stderr,"error: no config filename specified\n");
        return usage();
    }

    if(strlen(mktvsimg.basedir) == 0) {
        char cur_dir[PATH_MAX];
        getcwd(cur_dir, sizeof(cur_dir));
        fprintf(stderr, "base dir : %s \n", cur_dir);
        strcpy(mktvsimg.basedir, cur_dir);
    }

    error = ini_parse(config_fn, handler, &mktvsimg);

    if (error < 0) {
        printf("Can't read '%s'!\n", config_fn);
        return 2;
    }
    else if (error) {
        printf("Bad config file (first error on line %d)!\n", error);
        return 3;
    }

    if(!strlen(mktvsimg.update_fn))
    {
        fprintf(stderr, "error: no output filename specified\n");
        return 1;
    }

#if 0
    if(!access(mktvsimg.update_fn, F_OK))
    {
        printf("\nAlready %s exist! Do you want to continue? (N/y) ", mktvsimg.update_fn);
        char c = getchar();
        printf("\n");
        if( c == 'N' || c == '\n' || c == '\r')
        {
            printf("Canceled!!\n");
            return -1;
        }
    }
#endif

    if(!mktvsimg.tvs_img.page_size)
    {
        mktvsimg.tvs_img.page_size = 4096;
    }

    hdr = &mktvsimg.tvs_img;

    memcpy(hdr->magic, MAGIC, MAGIC_SIZE);
    hdr->hdr_size = sizeof(TVS_IMG);

    sprintf(update_fn_tmp, "%s.tmp", mktvsimg.update_fn);
    fd = open(update_fn_tmp, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
        fprintf(stderr,"error: could not create '%s'\n", mktvsimg.update_fn);
        return 1;
    }

    // write img header info
    if(write(fd, hdr, sizeof(TVS_IMG)) != sizeof(TVS_IMG)) goto fail;
    if(write_padding(fd, hdr->page_size, sizeof(TVS_IMG))) goto fail;

    for(i=0; i<hdr->img_num; i++)
    {
        LOGI("--> [%d] %s \n", i, hdr->img[i].flash_part_name);
        if(!strcmp("factory_settings", hdr->img[i].flash_part_name)) continue;
        file_write_size = load_file(mktvsimg.filename[i], i, &mktvsimg);

        if(!mktvsimg.file_buf[i])
        {
            printf("[%d] file open error(%s) \n", i, mktvsimg.filename[i]);
            return 0;
        }

        hdr->tot_img_size += hdr->img[i].size;

        off64_t cur_offset = lseek64(fd, 0, SEEK_CUR);
        mktvsimg.img_offset[i] = (uint32)cur_offset;
        //LOGI("[%d]: %s , start : 0x%08llx\n", i, hdr->img[i].flash_part_name, cur_offset);
        if(write(fd, mktvsimg.file_buf[i], file_write_size) != (ssize_t) file_write_size) goto fail;
        if(write_padding(fd, hdr->page_size, file_write_size)) goto fail;
        LOGI("<-- [%d] %s \n", i, hdr->img[i].flash_part_name);
    }

    lseek(fd, 0, SEEK_SET);

    if(write(fd, hdr, sizeof(TVS_IMG)) != sizeof(TVS_IMG)) goto fail;

    close(fd);

    LOGI("\n\nstrlen(mktvsimg.key_path) %d strlen(mktvsimg.key_prefix) %d \n", 
        strlen(mktvsimg.key_path), strlen(mktvsimg.key_prefix));

    if(strlen(mktvsimg.key_path) && strlen(mktvsimg.key_prefix))
    {
        printf("signing .......... \n");

        sprintf(path_pem, "%s/%s.pem", mktvsimg.key_path, mktvsimg.key_prefix);
        sprintf(path_pk8, "%s/%s.pk8", mktvsimg.key_path, mktvsimg.key_prefix);
        sprintf(path_x509, "%s/%s.x509.pem", mktvsimg.key_path, mktvsimg.key_prefix);
        if(access(path_pem, R_OK))
        {
            printf("generate pem ....... \n");
            generate_pem(path_pk8, path_pem);
        }
        else
        {
            printf("pem %s \n", path_pem);
        }

        generate_sign(update_fn_tmp, path_pem, path_x509, mktvsimg.update_fn);
    }
    else
    {
        char cmd[PATH_MAX], *work_dir, *file_name, path1[PATH_MAX], path2[PATH_MAX];
        sprintf(cmd, "mv %s %s", update_fn_tmp, mktvsimg.update_fn);
        LOGI("%s \n", cmd);
        system(cmd);

        strcpy(path1, mktvsimg.update_fn);
        strcpy(path2, mktvsimg.update_fn);
        work_dir = dirname(path1);
        file_name= basename(path2);
        sprintf(cmd, "cd %s; md5sum %s > %s.md5 ", work_dir, file_name, file_name);
        LOGI("%s \n", cmd);
        system(cmd);
    }

    print_img_info(&mktvsimg);
    printf("\n");

    return 0;

fail:
    unlink(mktvsimg.update_fn);
    close(fd);
    fprintf(stderr,"error: failed writing '%s': %s\n", mktvsimg.update_fn,
            strerror(errno));
    return 1;
}



#include <stdio.h>
#include <string.h>
#include "common.h"

int extract_partition(char* part_name, char* file_name)
{
    MKTVSIMG mktvsimg;
    TVS_IMG *hdr;
    TVS_IMG tvsimg;
    img_info *img;
    int read = 0, i;
    uint8 *buf;
    uint32 page_aligned_size;
    FILE* f;
    hdr = &tvsimg;

    LOGI("part_name %s, file_name %s \n", part_name, file_name);

    memset(&mktvsimg, 0, sizeof(MKTVSIMG));
    memset(&tvsimg, 0, sizeof(TVS_IMG));

    if(!file_name)
    {
        fprintf(stderr, "file name is NULL \n");
        return -1;
    }

    f = fopen(file_name, "rb");
    if (!f)
    {
        printf("open fail %s\n", file_name);
        return -1;
    }

    read = fread(&tvsimg, 1, sizeof(TVS_IMG), f);
    if(read != sizeof(TVS_IMG))
    {
        fprintf(stderr, "%s corrupted! \n", file_name);
        return -2;
    }

    fseek(f, PAGE_SIZE, SEEK_SET);
    long pos = ftell(f);

    memcpy(&mktvsimg.tvs_img, &tvsimg, sizeof(TVS_IMG));

    print_img_info(&mktvsimg);

    for(i=0; i<hdr->img_num; i++)
    {
        img = &hdr->img[i];

        LOGI("[%d] offset: 0x%08lx %s \n", i, pos,  img->flash_part_name);
        if(!strcmp(part_name, img->flash_part_name))
        {
            LOGI("<---------------------------------- \n");
            int size, is_compress = GET_COMPRESS(img->flag);
            if(is_compress) size = img->uncomp_size;
            else size = img->size;
            buf = malloc(size);

            if(!strcmp(part_name, "recovery") || !strcmp(part_name, "recovery_backup"))
            {
                fread(buf, 1, sizeof(DLDR), f);
            }
            uint32 n = fread(buf, 1, img->size, f);
            fclose(f);
            if(n != img->size)
            {
                printf("read fail (return %d) \n", n);
                break;
            }

            char ext_file_name[512];
            if(is_compress)
                sprintf(ext_file_name, "%s_%s.gz", file_name, img->flash_part_name);
            else
                sprintf(ext_file_name, "%s_%s", file_name, img->flash_part_name);

            printf("extract file name : %s \n", ext_file_name);
            
            FILE *ef = fopen(ext_file_name, "wb");
            n = fwrite(buf, 1, img->size, ef);
            fclose(ef);
            if(n != img->size)
            {
                printf("write fail (return %d) \n", n);
                break;
            }
            if(is_compress)
            {
                char cmd[512];
                sprintf(cmd, "gzip -d %s", ext_file_name);
                system(cmd);
            }
        }

        page_aligned_size = get_page_aligned_size(img->size, PAGE_SIZE);

        fseek(f, page_aligned_size, SEEK_CUR);
        pos = ftell(f);
    }

    //fclose(f);
}


#ifndef _COMMON_H
#define _COMMON_H

#include "../../../lge/library/dloader/tvsimg.h"
#include "../../../lge/library/dloader/prt_hdr.h"

#define USE_LOG         0

#if USE_LOG
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)
#define LOGP(...) fprintf(stdout, __VA_ARGS__)
#define LOGi(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOGE(...)
#define LOGW(...)
#define LOGI(...)
#define LOGi(...)
#define LOGP(...) fprintf(stdout, __VA_ARGS__)
#endif

void print_hash(uint8 *hash);
uint32 get_ver_from_string(const char* ver);
uint32 get_version(const char* basedir, const char* fn);
uint32 get_page_aligned_size(unsigned int size, int pagesize);

#endif



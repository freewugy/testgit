#ifndef __COMMON_H_
#define __COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define LOGE(...) fprintf(stderr, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stderr, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stderr, "I:" __VA_ARGS__)

#define TRACE_ENTER() fprintf(stderr, "I: --> %s : %d\n",  __FUNCTION__, __LINE__)
#define TRACE()       fprintf(stderr, "I: %s : %d\n",  __FUNCTION__, __LINE__)
#define TRACE_LEAVE() fprintf(stderr, "I: <-- %s : %d\n",  __FUNCTION__, __LINE__)
#endif

#define USE_FILE_KEY        1
#define USE_AES_128_CBC

#if defined(USE_AES_256_CBC)
#define CRYPT_METHOD        EVP_aes_256_cbc()
#define KEY_LENGTH          2
#elif defined(USE_AES_128_CBC)
#define CRYPT_METHOD        EVP_aes_128_cbc()
#define KEY_LENGTH          4
#endif


#ifdef __cplusplus
}
#endif

#endif

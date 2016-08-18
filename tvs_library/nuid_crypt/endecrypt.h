
#ifndef __ENDECRYPT_H_
#define __ENDECRYPT_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
        unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
        unsigned char *iv, unsigned char *plaintext);

//#ifdef __cplusplus
//}
//#endif

#endif


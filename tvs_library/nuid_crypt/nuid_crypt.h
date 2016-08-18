#ifndef __NUID_CRYPT_H_
#define __NUID_CRYPT_H_

int generate_encrypt_tvs_sys_key(void);
int decrypt_enc_sys(void);
int mount_enc(unsigned char* NUID);

#endif



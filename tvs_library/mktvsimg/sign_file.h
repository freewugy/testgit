
#ifndef _SIGN_FILE_H
#define _SIGN_FILE_H

int generate_pem(char* path_pk, char* path_pem);
int generate_sign(char* path_src, char* path_pk_pem, char* path_cert, char* path_signed_file);

#endif

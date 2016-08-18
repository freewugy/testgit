
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

/*
 * description: generate pem file from pk
 * IN
 *    path_pk: *.pk8
 * OUT
 *    path_pem: *.pem
 */
int generate_pem(char* path_pk, char* path_pem)
{
    int ret = 0;
    char cmd[1024];

    sprintf(cmd, "openssl pkcs8 -inform DER -nocrypt -in %s -out %s \n", 
            path_pk, path_pem);

    ret = system(cmd);
    if(ret)
    {
        perror("generate_pem");
    }
    else
    {
        fprintf(stderr, "generate pem OK \n");
    }

    return ret;
}

/*
 * description: 
 * IN
 *    path_src: to be signed file path, ex) update.bin
 *    path_pk_pem: *.pem file path
 *    path_cert: *.x509.pem file path
 * OUT
 *    path_sign_file: sign file
 */
int generate_sign(char* path_src, char* path_pk_pem, char* path_cert, char* path_signed_file)
{
    struct stat st;
    int ret = 0, fd;
    char cmd[1024], path_sign_file[PATH_MAX];
    uint32 size;

    sprintf(path_sign_file, "%s.sign", path_src);

    sprintf(cmd, "openssl smime -sign -md sha1 -binary -nocerts -noattr -in %s -out %s \
-outform der -inkey %s -signer %s", path_src, path_sign_file, path_pk_pem, path_cert);

    ret = system(cmd);
    if(ret)
        perror("generate_sign");

    sprintf(cmd, "cat %s %s > %s", path_src, path_sign_file, path_signed_file);

    ret = system(cmd);
    if(ret)
        perror("generate_sign");

    stat(path_sign_file, &st);
    LOGI("sign file size(%s): %lld \n", path_sign_file, st.st_size);

    fd = open(path_signed_file, O_WRONLY | O_APPEND, 0644);
    if(fd < 0) {
        fprintf(stderr,"error: could not create '%s'\n", path_signed_file);
        return 1;
    }

    size = st.st_size + 4;
    write(fd, &size, 4);
    close(fd);

    return ret;
}



#define LOG_TAG "nxserver_crypt"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"
#include "endecrypt.h"
#if defined(ST950I_LN)
#include "st950i_ln_private.key.plain.h"
#elif defined(SH960C_LN)
#include "sh960c_ln_private.key.plain.h"
#endif
#include "nuid.h"
#include <private/android_filesystem_config.h>

#define KEY_FILE_PATH       "/factory_settings/tvs_sys.key"
#define CHECK_RETURN(f)     do { if(f) return -1; } while(0)

#if defined(ST950I_LN)
#define LUKSOPEN_CMD        "cryptsetup.static luksOpen /dev/block/platform/rdb.7/by-name/system " \
                            "cryptsystem --readonly --key-file=-"
unsigned char *iv = (unsigned char*)"ST950ILNST950ILN";
#elif defined(SH960C_LN)
#define LUKSOPEN_CMD        "cryptsetup.static luksOpen /dev/block/platform/rdb.4/by-name/system " \
                            "cryptsystem --readonly --key-file=-"
unsigned char *iv = (unsigned char*)"SH960CLNSH960CLN";
#else
#error "It should be define SH960C-LN or ST950I-LN"
#endif

static int run_system_cmd(char *cmd)
{
    if(system(cmd))
    {
        ALOGE("Error: %s \n", cmd);
        return -1;
    }

    return 0;
}

static int open_file(char* file, unsigned char* key, int *len)
{
  int fd;
  struct stat fs;

  fd = open(file, O_RDONLY);

  if(fd < 0)
  {
      ALOGE("file open failed! %s \n", file);
      return -1;
  }

  fstat(fd, &fs);

  *len = fs.st_size;

  read(fd, key, *len);

  close(fd);

  return 0;
}

static int generate_file(char *file_path, unsigned char* text, int len)
{
  int fd;
  struct stat fs;
  char enc_file_ext[256];
  char cmd[512];

  ALOGI("generate_file(%s, len %d) \n", file_path, len);

  if(!strcmp("-", file_path))
  {
      for(int i=0; i<len; i++)
          fprintf(stdout, "%c", text[i]);
      return 0;
  }

  CHECK_RETURN(run_system_cmd("mount -o remount,rw /factory_settings"));
 
  fd = open(file_path, O_WRONLY | O_CREAT, 0644);

  if(fd < 0)
  {
      ALOGE("file open failed! %s \n", file_path);
      return -1;
  }

  write(fd, text, len);

  close(fd);

  ALOGI("generate_file() return 0\n");
  return 0;
}

static int init_openssl(void)
{
  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);
  return 0;
}

static int exit_openssl(void)
{
  /* Clean up */
  EVP_cleanup();
  ERR_free_strings();

  return 0;
}

int generate_encrypt_tvs_sys_key(void)
{
    /* A 256 bit key */
    unsigned char key[33] = {"01234567890123456789012345678901"};
    unsigned char ciphertext[4*1024];
    unsigned char NUID[8];
    int ciphertext_len;

    ALOGI("----> encrypt \n");

    if(secGetNuid(NUID))
    {
        return -1;
    }
    for(int i=0; i<KEY_LENGTH; i++)
    {
        memcpy(key+i*4, NUID, 4);
    }

    init_openssl();

    /* Encrypt the plainkey */
    ciphertext_len = encrypt((unsigned char*)private_key_plain, private_key_plain_size, key, iv,
            ciphertext);

    exit_openssl();

    if(generate_file((char*)KEY_FILE_PATH, ciphertext, ciphertext_len))
    {
        return -2;
    }

    ALOGI("<---- encrypt \n");
    return 0;
}

static int mount_enc_sys(unsigned char* decrypt_text, int len)
{
    char cmd[512];

    ALOGI("---> mount_enc_sys\n");

    FILE* fChild = popen(LUKSOPEN_CMD, "wb"); 

    if (NULL == fChild)
    {
        ALOGE("open cryptsetup failed \n");
        return -1;
    }

    fwrite(decrypt_text, 1, len, fChild);

    pclose(fChild);

    chown("/dev/mapper/cryptsystem", AID_ROOT, AID_SYSTEM);
    chmod("/dev/mapper/cryptsystem", 0600);

#if 1
    CHECK_RETURN(mount(NULL, "/", "none", MS_REMOUNT, NULL));
    CHECK_RETURN(mount("/dev/mapper/cryptsystem",   "/system_encrypted","ext4", MS_RDONLY, NULL));
    CHECK_RETURN(mount("/system_encrypted/app",     "/system/app",      "none", MS_BIND, NULL));
    CHECK_RETURN(mount("/system_encrypted/fonts",   "/system/fonts",    "none", MS_BIND, NULL));
    CHECK_RETURN(mount("/system_encrypted/media",   "/system/media",    "none", MS_BIND, NULL));
    CHECK_RETURN(mount("/system_encrypted/priv-app","/system/priv-app", "none", MS_BIND, NULL));
    CHECK_RETURN(mount("/system_encrypted/tts",     "/system/tts",      "none", MS_BIND, NULL));
    CHECK_RETURN(mount("/system_encrypted/usr",     "/system/usr",      "none", MS_BIND, NULL));
    CHECK_RETURN(mount(NULL, "/", "none", MS_REMOUNT|MS_RDONLY, NULL));
#else
    CHECK_RETURN(run_system_cmd("vdc tvs remount rw /"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount ext4 /dev/mapper/cryptsystem     /system_encrypted"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/app       /system/app"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/fonts     /system/fonts"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/media     /system/media"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/priv-app  /system/priv-app"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/tts       /system/tts"));
    CHECK_RETURN(run_system_cmd("vdc tvs mount bind /system_encrypted/usr       /system/usr"));
    CHECK_RETURN(run_system_cmd("vdc tvs remount ro /"));
#endif

    ALOGI("<--- mount_enc_sys\n");
    return 0;
}

int decrypt_key_file(unsigned char* NUID, unsigned char *decr_text, int *decr_len)
{
    unsigned char ciphertext[4*1024];
    unsigned char key[33] = {"01234567890123456789012345678901"};
    int ciphertext_len, rc;

    for(int i=0; i<KEY_LENGTH; i++)
    {
        memcpy(key+i*4, NUID, 4);
    }

    if(open_file((char*)KEY_FILE_PATH, ciphertext, &ciphertext_len) < 0)
    {
        ALOGE("open file %s error", KEY_FILE_PATH);
        return -1;
    }

    init_openssl();

    /* Decrypt the ciphertext */
    *decr_len = decrypt(ciphertext, ciphertext_len, key, iv, decr_text);

    exit_openssl();

    return 0;
}

int mount_enc(unsigned char* NUID)
{
    unsigned char decryptedtext[4*1024];
    int decryptedtext_len, rc = 0;

    //ALOGI("---> %s(%02x %02x %02x %02x)\n", __func__, NUID[0], NUID[1], NUID[2], NUID[3]);
    rc = decrypt_key_file(NUID, decryptedtext, &decryptedtext_len);
    if(rc) 
    {
        ALOGI("decrypt_key_file failed %d\n", rc);
        return -1;
    }

    rc = mount_enc_sys(decryptedtext, decryptedtext_len);

    ALOGI("%s: rc %d\n", __func__, rc);
    return rc;
}

int decrypt_enc_sys(void)
{
    unsigned char NUID[8];
    char cmd[256];
    int rc = 0;

    ALOGI("---> decrypt_cryptsetup \n");

    secGetNuid(NUID);

    sprintf(cmd, "vdc tvs mount_enc %02x %02x %02x %02x", NUID[0], NUID[1], NUID[2], NUID[3]);
    //ALOGI("cmd: %s \n", cmd);

    CHECK_RETURN(run_system_cmd(cmd));

    ALOGI("<--- decrypt_cryptsetup \n");
    return rc;
}



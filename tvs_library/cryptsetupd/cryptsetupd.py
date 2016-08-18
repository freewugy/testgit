#!/usr/bin/env python
 
import socket, select
import os,sys,subprocess
import time

VERSION="v1.2.0"
CRYPTSETUPD_TITLE="cryptsetupd " + VERSION
PRIVATE_KEY_PATH_ST950I_LN="./tvs_sys.key.plain.st950i-ln"
PRIVATE_KEY_PATH_SH960C_LN="./tvs_sys.key.plain.sh960c-ln"
VERBOSE='n'
MODEL="ST950I-LN"
ENC_EMPTY   = "./system.encrypted.empty.st950i-ln"
PRIVATE_KEY = PRIVATE_KEY_PATH_ST950I_LN
LOOP_DEV    = "/dev/loop0"
MAPPER_NAME = "cryptsystem_st950i-ln"
# Encrypted file system block count : (system partition size / 4096) - 1
ENCRYPTED_FS_BLK_COUNT = "148479"       # 580MB - 4096bytes

def check_cryptsetup_install():
    ret = os.system("cryptsetup --version > /dev/null")
    return ret

def log(user_id, log_msg):
    global MODEL
    now = time.localtime()
    cur_date = "%04d-%02d-%02d" % (now.tm_year, now.tm_mon, now.tm_mday)
    cur_time = "%02d:%02d:%02d" % (now.tm_hour, now.tm_min, now.tm_sec)
    log_file = "./log/cryptsetupd_" + MODEL + "_%s.log" % cur_date
    if(False == os.path.exists("./log")):
        os.system("mkdir ./log")
    msg = '%s %s [%s] %s' % (cur_date, cur_time, user_id, log_msg)
    if VERBOSE == 'y':
        print(msg)
    cmd = 'echo "%s" >> %s' % (msg, log_file)
    #print(cmd)
    os.system(cmd)

def log0(user_id, log_msg):
    global MODEL
    now = time.localtime()
    cur_date = "%04d-%02d-%02d" % (now.tm_year, now.tm_mon, now.tm_mday)
    log_file = "./log/cryptsetupd_" + MODEL + "_%s.log" % cur_date
    if VERBOSE == 'y':
        print(log_msg)
    cmd = 'echo "%s" >> %s' % (log_msg, log_file)
    if(False == os.path.exists("./log")):
        os.system("mkdir ./log")
    os.system(cmd)

def check_return(user_id, cmd):
    #cmd_str = "%s > /dev/null 2>&1" % cmd
    try:
        ret_str = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        VERBOSE = 'n'
        log(user_id, cmd)
        if len(ret_str):
            log0(user_id, ret_str)
        os.system("sleep 1")
        ret = 0
    except subprocess.CalledProcessError, e:
        VERBOSE = 'y'
        log(user_id, "%s return FAIL" % cmd)
        ret = 1

    return ret

def clean_cryptsetup():
    global MAPPER_NAME
    global LOOP_DEV

    ret = check_return("root", "cryptsetup luksClose " + MAPPER_NAME)
    #if(ret) :
    #    return ret;
    ret = check_return("root", "losetup -d " + LOOP_DEV)
    if(ret) :
        ret = os.system("dmsetup remove_all -f ")
 
def init_env(model):
    global ENC_EMPTY
    global PRIVATE_KEY
    global MAPPER_NAME
    global LOOP_DEV

    if model == "ST950I-LN" :
        ENC_EMPTY   = "./system.encrypted.empty.st950i-ln"
        PRIVATE_KEY = PRIVATE_KEY_PATH_ST950I_LN
        LOOP_DEV    = "/dev/loop0"
        MAPPER_NAME = "cryptsystem_st950i-ln"
    elif model == "SH960C-LN" :
        ENC_EMPTY   = "./system.encrypted.empty.sh960c-ln"
        PRIVATE_KEY = PRIVATE_KEY_PATH_SH960C_LN
        LOOP_DEV    = "/dev/loop1"
        MAPPER_NAME = "cryptsystem_sh960c-ln"
    log("root", "ENC_EMPTY   : " + ENC_EMPTY);
    log("root", "PRIVATE_KEY : " + PRIVATE_KEY);
    log("root", "LOOP_DEV    : " + LOOP_DEV);
    log("root", "MAPPER_NAME : " + MAPPER_NAME);

def init_encrypted_file():
    global ENC_EMPTY
    global PRIVATE_KEY
    global MAPPER_NAME
    global LOOP_DEV

    ret = check_return("root", "dd if=/dev/zero of=" + ENC_EMPTY + " bs=4096 count=" + ENCRYPTED_FS_BLK_COUNT) # 579MB
    if(ret) :
        return ret;
    ret = check_return("root", "losetup " + LOOP_DEV + " " + ENC_EMPTY)
    if(ret) :
        return ret;
    ret = check_return("root", "cryptsetup luksFormat -i 1 " + LOOP_DEV + " -c aes-plain --key-file=" + PRIVATE_KEY + " --batch-mode -v")
    if(ret) :
        return ret;
    ret = check_return("root", "cryptsetup luksOpen " + LOOP_DEV + " " + MAPPER_NAME + " --key-file=" + PRIVATE_KEY)
    if(ret) :
        return ret;
    ret = check_return("root", "mke2fs -O ^has_journal,uninit_bg,resize_inode,extent,dir_index -L system.encrypted -FF /dev/mapper/" + MAPPER_NAME)

    clean_cryptsetup()

    return ret;

def cp_to_encrypted(user_id, path):

    ret = check_return(user_id, "cp -rf %s/system/app %s/system_encrypted/" % (path,path))
    if(ret):
        return ret
    ret = check_return(user_id, "cp -rf %s/system/fonts %s/system_encrypted/" % (path,path))
    if(ret):
        return ret
    ret = check_return(user_id, "cp -rf %s/system/media %s/system_encrypted/" % (path,path))
    if(ret):
        return ret
    ret = check_return(user_id, "cp -rf %s/system/priv-app %s/system_encrypted/" % (path,path))
    if(ret):
        return ret
    ret = check_return(user_id, "cp -rf %s/system/tts %s/system_encrypted/" % (path,path))
    if(ret):
        return ret
    ret = check_return(user_id, "cp -rf %s/system/usr %s/system_encrypted/" % (path,path))
    if(ret):
        return ret

def make_encrypted_system(user_id, path):
    global ENC_EMPTY
    global PRIVATE_KEY
    global MAPPER_NAME
    global LOOP_DEV

    log(user_id, "make_encrypted_system started ...")

    #print "user id : " + user_id
    #print "path    : " + path

    ret = check_return(user_id, "rm -rf %s/system.encrypted" % path)
    if(ret):
        return ret
    ret = check_return(user_id, "cp " + ENC_EMPTY + " %s/system.encrypted" % path)
    if(ret):
        return ret
    ret = check_return(user_id, "losetup " + LOOP_DEV + " " + path + "/system.encrypted")
    if(ret):
        return ret
    ret = check_return(user_id, "cryptsetup luksOpen " + LOOP_DEV + " " + MAPPER_NAME + " --key-file=" + PRIVATE_KEY)
    if(ret):
        return ret
    ret = check_return(user_id, "rm -rf %s/system_encrypted" % path)
    if(ret):
        return ret
    ret = check_return(user_id, "mkdir %s/system_encrypted" % path)
    if(ret):
        return ret
    ret = check_return(user_id, "mount /dev/mapper/" + MAPPER_NAME + " %s/system_encrypted" % path)
    if(ret):
        return ret
    ret = cp_to_encrypted(user_id, path)
    if(ret):
        return ret
    ret = check_return(user_id, "umount %s/system_encrypted" % path)
    if(ret):
        return ret
    ret = check_return(user_id, "cryptsetup luksClose " + MAPPER_NAME)
    if(ret):
        return ret
    ret = check_return(user_id, "losetup -d " + LOOP_DEV)
    if(ret):
        return ret
    #ret = check_return(user_id, "chown -R %s.%s %s/system_encrypted" % (user_id, user_id, path))
    #if(ret):
    #    return ret
    ret = check_return(user_id, "chown %s.%s %s/system.encrypted" % (user_id, user_id, path))
    if(ret):
        return ret
    log(user_id, "make_encrypted_system leave...")
    return 0

#Function to broadcast chat messages to all connected clients
def broadcast_data (sock, message):
    #Do not send the message to master socket and the client who has send us the message
    for socket in CONNECTION_LIST:
        if socket != server_socket and socket != sock :
            try :
                socket.send(message)
            except :
                # broken socket connection may be, chat client pressed ctrl+c for example
                socket.close()
                CONNECTION_LIST.remove(socket)
 
###################################################################################################
if __name__ == "__main__":
    if(len(sys.argv) == 1) :
        print "Usage : python cryptsetupd.py [ST950I-LN | SH960C-LN]"
        sys.exit(1)
     
    MODEL = sys.argv[1]

    if MODEL != "ST950I-LN" and MODEL != "SH960C-LN" :
        log("root", "model name is undefined")
        sys.exit(1)

    log0("root", "\n");
    log("root", "="*73);
    log("root", "===================== " + CRYPTSETUPD_TITLE +  " (" + MODEL + ") ====================")
    log("root", "="*73);

    if(check_cryptsetup_install()):
        print "cryptsetup NOT installed. install cryptsetup 'sudo apt-get install cryptsetup'"
        sys.exit(1)

    # init
    init_env(MODEL)

    # List to keep track of socket descriptors
    CONNECTION_LIST = []
    RECV_BUFFER = 4096 # Advisable to keep it as an exponent of 2
    if MODEL == "ST950I-LN" :
        PORT = 45000
        ENCRYPTED_FS_BLK_COUNT = "148479"       # 580MB - 4096bytes
    elif MODEL == "SH960C-LN" :
        PORT = 45001
        ENCRYPTED_FS_BLK_COUNT = "166399"       # 650MB - 4096bytes

    ret = init_encrypted_file()
    if(ret):
        log("root", "init_encrypted_file failed")
        sys.exit(1)

 
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # this has no effect, why ?
    #server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(("0.0.0.0", PORT))
    server_socket.listen(10)
 
    # Add server socket to the list of readable connections
    CONNECTION_LIST.append(server_socket)
 
    log("root", "="*40);
    msg = "cryptsetup server started on port " + str(PORT)
    log("root", msg);
    log("root", "="*40);
 
    # sh960c-ln.pid / st950i-ln.pid
    if MODEL == "ST950I-LN" :
        os.system("echo %s > %s" % (os.getpid(), "st950i-ln.pid"))
    elif MODEL == "SH960C-LN" :
        os.system("echo %s > %s" % (os.getpid(), "sh960c-ln.pid"))

    while 1:
        # Get the list sockets which are ready to be read through select
        read_sockets,write_sockets,error_sockets = select.select(CONNECTION_LIST,[],[])
 
        for sock in read_sockets:
            #New connection
            if sock == server_socket:
                # Handle the case in which there is a new connection recieved through server_socket
                sockfd, addr = server_socket.accept()
                CONNECTION_LIST.append(sockfd)
                msg = "Client (%s,%s) connected" % addr
                log("root", msg)
                 
                #broadcast_data(sockfd, "[%s:%s] entered room\n" % addr)
             
            #Some incoming message from a client
            else:
                # Data recieved from client, process it
                try:
                    #In Windows, sometimes when a TCP program closes abruptly,
                    # a "Connection reset by peer" exception will be thrown
                    data = sock.recv(RECV_BUFFER)

                    ###############################################
                    print(data)
                    user_info = data.split()
                    user_id = user_info[0]
                    path = user_info[1]
                    len_path = len(path)
                    if('/' == path[len_path-1]):
                        path = path[:-1]
                    #print("user_id: " + user_id)
                    #print("path: " + path)
                    ###############################################

                    if data:
                        ret = make_encrypted_system(user_id, path)
                        if(ret):
                            sock.send("FAIL")                
                            clean_cryptsetup()
                            log(user_id, "mount failed return %s" % ret)
                        else:
                            sock.send("OK")                
                            log(user_id, "make_encrypted_system return %s" % ret)
                    break
                except socket.timeout:
                    print "timeout error"
                    continue
                except:
                    e0 = sys.exc_info()[0]
                    e1 = sys.exc_info()[1]
                    e2 = sys.exc_info()[2]
                    #broadcast_data(sock, "Client (%s, %s) is offline" % addr)
                    log("root", "except: %s %s %s" % (e0, e1, e2))
                    log("root", "Client (%s, %s) is offline" % addr)
                    sock.close()
                    CONNECTION_LIST.remove(sock)
                    continue
     
    server_socket.close()


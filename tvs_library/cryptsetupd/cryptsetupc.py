#!/usr/bin/env python

import socket, select, string, sys
 
def prompt() :
    sys.stdout.write('<You> ')
    sys.stdout.flush()
 
#main function
if __name__ == "__main__":
     
    if(len(sys.argv) < 4) :
        print 'Usage : python cryptsetupc.py model user_id android_product_out_path'
        sys.exit(1)
     
    MODEL = sys.argv[1]

    if MODEL != "ST950I-LN" and MODEL != "SH960C-LN" :
        log("root", "model name is undefined")
        sys.exit(1)

    user = sys.argv[2]
    path = sys.argv[3]
    host = "localhost"

    if MODEL == "ST950I-LN" :
        PORT = 45000
    elif MODEL == "SH960C-LN" :
        PORT = 45001
 
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(None)
     
    # connect to remote host
    try :
        s.connect((host, PORT))
    except :
        print 'Unable to connect'
        sys.exit(1)
     
    #print 'Connected to remote host. Start sending messages'
    #prompt()
     
    # send user info
    user_info = user + " " + path
    s.send(user_info)

    while 1:
        #socket_list = [sys.stdin, s]
         
        # Get the list sockets which are readable
        #read_sockets, write_sockets, error_sockets = select.select(socket_list , [], [])
         
        #for sock in read_sockets:
            #incoming message from remote server
        try:
            data = s.recv(4096)
            if not data :
                print '\nDisconnected from build server'
                sys.exit(-1)
            else :
                #print data
                #sys.stdout.write(data)
                if(data == "OK"):
                    sys.exit(0)
                else:
                    #if(data == "FAIL"):
                    sys.exit(1)

        except socket.timeout:
            print "timeout error"
        except Exception as e:
            print e
            #user entered a message
               

#!/usr/bin/env python

import traceback
import socket
import thread
import time
import ssl
import sys
import os

BUFFSIZE = 4096

def printf(format,*args): sys.stdout.write(format%args)

def usage(prog):
    printf("usage is %s <host> <port> <keyFile> <crtFile>\n",prog)
    printf("\n")
    printf("start an ssl server and echo back the data sent by a client\n")
    printf("in a line buffered fasion.\n")

def excuse():
    except_message = traceback.format_exc()
    stack_message  = traceback.format_stack()
    return except_message + " " + str(stack_message)


def bury_the_dead():
  while True:
    try:
      (pid,stats) = os.waitpid(-1,os.WNOHANG)
      if (pid,stats) == (0,0):
        break
      else:
        printf("pid[%i] buried[%i]\n",pid,stats)
    except:
      break

def child_server(cs):
    while True:
        data = cs.read(BUFFSIZE)
        if len(data) == 0:
            return
        cs.write(data)

def main(argv):
    prog = os.path.basename(argv[0])
    if len(argv) < 5:
        usage(prog)
        sys.exit()
    host = argv[1]
    port = int(argv[2])
    keyfile = os.path.expanduser(argv[3])
    crtfile = os.path.expanduser(argv[4])
    fmt="starting server %s:%i with keyFile %s crtFile %s\n"
    printf(fmt,host,port,keyfile,crtfile)
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.bind((host,port))
    s.listen(10)
    ss = ssl.wrap_socket(s,keyfile=keyfile,certfile=crtfile,server_side=True)

    while True:
        bury_the_dead()
        (cs,a) = ss.accept()
        printf("connect: %s\n",a)
        pid = os.fork()
        if pid == 0:
            printf("Child processing %s\n",a)
            sys.stdout.flush()
            child_server(cs)
            cs.close()
            ss.close()
            sys.exit()
        else:
            cs.close()
            printf("relistening\n")

if __name__ == "__main__":
    main(sys.argv)

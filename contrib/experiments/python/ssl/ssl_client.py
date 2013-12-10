#!/usr/bin/env python

import socket
import ssl
import time
import sys
import os

BUFFSIZE = 4096

def printf(format,*args): sys.stdout.write(format%args)
    

def usage(prog):
    printf("usage is %s <host> <port> <sleep>\n",prog)
    printf("\n")
    printf("start an ssl client and send a few lines of data\n")

def bury_the_dead():
  while True:
    try:
      (pid,stats) = os.waitpid(-1,os.WNOHANG)
      if (pid,stats) == (0,0):
        break
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
    if len(argv) < 4:
        usage(prog)
        sys.exit()
    host = argv[1]
    port = int(argv[2])
    secs = float(argv[3])
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    cs = ssl.wrap_socket(s,cert_reqs=ssl.CERT_NONE)
    printf("Connect ")
    sys.stdout.flush()
    cs.connect((host,port))
    printf(" done");
    sys.stdout.flush()
    fp = cs.makefile("rw")
    fp.write("test %i\n"%os.getpid())
    fp.flush()
    printf("sleeping for %f seconds\n",secs)
    sys.stdout.flush()
    time.sleep(secs)
    line = fp.readline()
    printf("echoed: %s\n",line)
    sys.stdout.flush()
    fp.close()
    cs.close()

if __name__ == "__main__":
    main(sys.argv)

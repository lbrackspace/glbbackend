#!/usr/bin/env python

import socket
import sys
import os

BUFFSIZE = 1024*64

lo = 2130706432L

def printf(format,*args): sys.stdout.write(format%args)

def fprintf(fp,format,*args): fp.write(format%args)

baseFQDN="rackexp.org"

def intToIp(n):
    return "%i.%i.%i.%i"%(n>>24,(n>>16)&255,(n>>8)&255,n&255)

def drain(fp):
    line_count = 0
    byte_count = 0
    while True:
        line = fp.readline()
        line_count += 1
        byte_count += len(line)
        if line == "OVER\n":
            break
    return (line_count,byte_count)



def usage(prog):
    printf("usage is %s <ip> <port> <startDomain> <ndomains> <nIps>\n",prog)
    printf("\n")
    printf("connects to server and sends data\n")

def add_domain(fp,dn):
    fp.write("ADD_DOMAIN glb_%i.%s NONE\n"%(dn,baseFQDN))

def del_domain(fp,dn):
    fp.write("DEL_DOMAIN glb_%i.%s\n"%(dn,baseFQDN))

def snapshot(fp,dn,nIps):
    fp.write("SNAPSHOT glb_%i.%s"%(dn,baseFQDN))
    for i in xrange(0,nIps):
       fp.write(" 4-30-%s-0"%intToIp(lo+i))
    fp.write("\n")

def counts(fp,*dn):
    if len(dn)==0:
        fp.write("COUNTS\n")
    else:
        for i in dn:
            fp.write("COUNTS glb_%i.%s"%(dn,baseFQDN))

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

s.connect(("127.0.0.1",8888))
fp = s.makefile("rw")

for i in xrange(0,128):
    add_domain(fp,i)
    snapshot(fp,i,25)

fp.write("OVER\n")
fp.flush()
drain(fp)
sys.exit()


for i in xrange(0,128000):
    del_domain(fp,i)

nl = 0

ln = 0
while True:
    line = fp.readline()
    if line == "OVER\n":
        break
    ln += 1


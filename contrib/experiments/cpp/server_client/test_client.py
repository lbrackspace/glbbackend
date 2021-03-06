#!/usr/bin/env python

import socket
import sys
import os

BUFFSIZE = 1024*64

lo = 2130706432L

def printf(format,*args): sys.stdout.write(format%args)

def fprintf(fp,format,*args): fp.write(format%args)

def intToIp(n):
    return "%i.%i.%i.%i"%(n>>24,(n>>16)&255,(n>>8)&255,n&255)

def usage(prog):
    printf("usage is %s <ip> <port> <startDomain> <ndomains> <nIps>\n",prog)
    printf("\n")
    printf("connects to server and sends data\n")

def add_domain(fp,dn):
    fp.write("ADD_DOMAIN glb_%i.rackexp.org NONE\n"%dn)

def del_domain(fp,dn):
    fp.write("DEL_DOMAIN glb_%i.rackexp.org\n"%dn)

def snapshot(fp,dn,nIps):
    fp.write("SNAPSHOT glb_%i.rackexp.org"%dn)
    for i in xrange(0,nIps):
       fp.write(" 4-30-%s-0"%intToIp(lo+i))
    fp.write("\n")


s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

s.connect(("wtf.rackexp.org",8888))
fp = s.makefile("rw")

for i in xrange(0,1000000):
    add_domain(fp,i)
    snapshot(fp,i,25)

for i in xrange(0,1000000):
    del_domain(fp,i)

nl = 0

while(fp.readline() != "OVER\n"):
    pass

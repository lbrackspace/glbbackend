#!/usr/bin/env python

import socket
import json
import sys
import os

BUFFSIZE = 1024*64

lo = 2130706432L
lo6 = 252<<120

hex="0123456789abcdef"

def printf(format,*args): sys.stdout.write(format%args)

def fprintf(fp,format,*args): fp.write(format%args)

baseFQDN="glb.rackexp.org"

def intToIp(n):
    return "%i.%i.%i.%i"%(n>>24,(n>>16)&255,(n>>8)&255,n&255)

def intToIp6(n):
    out = ""
    for i in xrange(31,-1,-1):
        out += hex[(n>>(i*4))&0x0f]
        if i%4==0 and i != 0:
            out +=":"
    return out


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
       fp.write(" 4-30-%s-1"%intToIp(lo + i))
       fp.write(" 6-30-%s-1"%intToIp6(lo6 + i))
       fp.write(" 3-15-NOT_AN_IP-1")
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

for i in xrange(0,2):
    add_domain(fp,i)
    snapshot(fp,i,2)

for i in xrange(1,4):
    fp.write("ADD_DOMAIN ns%i.%s NONE\n"%(i,baseFQDN));
    fp.write("SNAPSHOT ns%i.%s 4-30-23.253.111.77-1"%(i,baseFQDN))
    fp.write(" 6-30-%s-1"%intToIp6(lo6+i))
    fp.write(" 44-30-BAD_IP-1\n")

soa="ns1.%s. root.%s. 2013102907 28800 14400 3600000 300"%(baseFQDN,baseFQDN)
fp.write("SET_SOA %s %s\n"%(baseFQDN,soa))
fp.write("SET_NS")
for i in xrange(1,4):
    fp.write(" ns%i.%s"%(i,baseFQDN))

fp.write("\nOVER\n")
fp.flush()


drain(fp)
fp.close()
s.close()
sys.exit()


for i in xrange(0,4):
    del_domain(fp,i)

nl = 0

ln = 0
while True:
    line = fp.readline()
    if line == "OVER\n":
        break
    ln += 1


#!/usr/bin/env python

#Sender
import pika
import networkq

def lm(meth):
    for m in sorted(dir(meth)): print m

def dumpMsgs(nq,nMessages,qname='test',qMesg='testMessage'):
    for i in xrange(0,nMessages):
        if i % 1024 == 0:
            print "%i of %i"%(i,nMessages)
        nq.send(qname,"%s: %i"%(qMesg,i))

con = pika.BlockingConnection(pika.ConnectionParameters(host="localhost"))
chan = con.channel()
chan.queue_declare(queue='test',durable=True)
nq = networkq.ChannelQueue(chan)

#dumpMsgs(nq,1024*1024)


#Receiver
import pika
import networkq

def lm(meth):
    for m in sorted(dir(meth)): print m

def getTags(nq,n,qname='test'):
    tags = []
    for i in xrange(0,n):
        if i%1024 == 0:
            print "%i of %i"%(i,n)
        (method,properties,body) = nq.chan.basic_get(queue=qname)
        if method.NAME == "Basic.GetEmpty":
            return tags
        tags.append(method.delivery_tag)
    return tags

def ackTags(nq,tags,qname='test'):
    n = len(tags)
    i = 0
    for tag in tags:
        if i % 1024 == 0:
            print "%i of %i"%(i,n)
        nq.chan.basic_ack(delivery_tag=tag)
        i += 1


con = pika.BlockingConnection(pika.ConnectionParameters(host="localhost"))
chan = con.channel()
chan.queue_declare(queue='test',durable=True)
nq = networkq.ChannelQueue(chan)

m = nq.recv('test',1024)
#do your work with the messages
nq.ackAll(m)

(method,properties,body) = chan.basic_get(queue='test')

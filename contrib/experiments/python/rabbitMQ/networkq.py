#!/usr/bin/env python

import Queue
import pika
import thread
import time


class ChannelQueue(object):

    def __init__(self,chan):
        self.chan = chan

    def recv(self,queueName,n,ack=False):
        objList = []
        for i in xrange(0,n):
            if i % 1024 == 0:
                print "%i of %i"%(i,n)
            (method,properties,body) = self.chan.basic_get(queue=queueName)
            if method.NAME == "Basic.GetEmpty":
                return objList
            if ack==True:
                self.chan.basic_ack(delivery_tag=method.delivery_tag)
            objList.append([method,properties,body])
        return objList

    def send(self,queueName,msg):
        kw = {"exchange":"",
                  "routing_key":queueName,
                  "body":msg,
                  "properties":pika.BasicProperties(delivery_mode=2)}
        self.chan.basic_publish(**kw)

    def ackAll(self, objList):
        for (method,properties,body) in objList:
            self.chan.basic_ack(delivery_tag=method.delivery_tag)

    def ack(self,obj):
        self.chan.basic_ack(deliver_tag=obj.method.delivery_tag)

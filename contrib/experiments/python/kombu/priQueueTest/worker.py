#!/usr/bin/env python
from kombu.mixins import ConsumerMixin
from kombu.log import get_logger
from kombu.utils import kwdict, reprcall

from queues import task_queues, api_queue
from multiprocessing import Process, Queue as MPQueue, current_process, Value
from Queue import Queue
from ctypes import c_bool

import thread
import time

logger = get_logger(__name__)

class Worker(ConsumerMixin):
    handler_process = None

    def __init__(self, connection, max_size=1000, queue_time = 30):
        self.connection = connection
        self.internal_queue = Queue()
        self.max_size = Value('i',max_size)
        self.last_time = Value('d',time.time())
        self.queue_time = Value('i',queue_time)
        self.work_unit = Value('i',0)
        thread.start_new_thread(self.queue_thread, ("Timer Thread",))
        #self.handler_process = Process(target=self.queue_thread,
         #                       args=(self.internal_queue,))
        self.process = Value(c_bool, True)

##### START WORKER PROCESS #####

    def do_work(self, work, unit):
        work_time = 0.0002
        logger.info("<Processing Work Unit -- Sleeping for %d seconds to emulate work.>", len(work)*work_time)
        for item in work:
            time.sleep(work_time)
        logger.info("<Done with Work Unit %i.>", unit)

    def stopWorkerProcess(self):
        self.process.value = False
        if self.handler_process: self.handler_process.join()
        print "Joined child processes in preparation for shutdown."

##### END WORKER PROCESS #####

##### START MAIN PROCESS #####

    def queue_thread(self, name): 
        logger.info("Started %r.", name)
        while self.process.value:
            wait = self.last_time.value + self.queue_time.value - time.time()
            while self.process.value and wait > 0:
                #logger.info("%r sleeping for %f seconds...", name, wait)
                time.sleep(wait)
                wait = self.last_time.value + self.queue_time.value - time.time()

            self.handle_queue(name)

    def handle_queue(self, caller):
        logger.info("%r triggered handling of the queue.", caller)
        time_diff = time.time() - self.last_time.value
        self.last_time.value = time.time()
        messages = []
        while self.process.value and len(messages) < self.max_size.value and not self.internal_queue.empty():
                messages.append(self.internal_queue.get(timeout=0.1))

        logger.info("Got a block of %d messages %f seconds since the last run.",
                        len(messages), time_diff)
        if len(messages) > 0:
                #logger.info("<Function sendToPdns() would run here on %d messages.>",
                #                len(messages))
                if self.handler_process:
                    self.handler_process.join()
                self.handler_process = Process(target=self.do_work, args=(messages,self.work_unit.value,))
                self.handler_process.start()
                self.work_unit.value += 1
        #logger.info(messages)

    def get_consumers(self, Consumer, channel):
        queues = [Consumer(queues=api_queue,
                         accept=['pickle', 'json'],
                         callbacks=[self.process_message])]
        queues[0].qos(prefetch_count=self.max_size.value)
        #self.handler_process.start()
        return queues

    def process_message(self, body, message):
        #logger.info('Got message: %s', body)
        self.internal_queue.put(body)
        message.ack()
        if self.internal_queue.qsize() >= self.max_size.value:
                self.handle_queue("Callback")

##### END MAIN PROCESS #####

if __name__ == '__main__':
    from kombu import Connection, enable_insecure_serializers
    from kombu.utils.debug import setup_logging
    # setup root logger
    setup_logging(loglevel='INFO', loggers=[''])

    enable_insecure_serializers(choices=['pickle'])

    #with Connection('amqp://guest:guest@localhost//') as conn:
    with Connection('amqp://guest:guest@rabbitmq-server//') as conn:
        try:
            worker = Worker(conn, max_size=10000, queue_time = 15)
            worker.run()
        except KeyboardInterrupt:
            worker.stopWorkerProcess()
            print('Done.')

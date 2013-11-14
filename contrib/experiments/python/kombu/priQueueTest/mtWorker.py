#!/usr/bin/env python
from kombu.mixins import ConsumerMixin
from kombu.log import get_logger
from kombu.utils import kwdict, reprcall

from queues import task_queues, api_queue, monitor_queue
from multiprocessing import Process, Queue as MPQueue, current_process, Value
from Queue import Queue
from ctypes import c_bool
import time

logger = get_logger(__name__)

class Worker(ConsumerMixin):

    def __init__(self, connection, max_size=1000, queue_time = 30):
        self.connection = connection
        self.shared_queue = MPQueue()
        self.max_size = Value('i',max_size)
        self.last_time = Value('d',time.time())
        self.queue_time = Value('i',queue_time)
        #thread.start_new_thread(self.queue_thread, ("QueueThread",))
        self.handler_process = Process(target=self.queue_thread,
                                args=(max_size,queue_time,self.shared_queue))
        self.process = Value(c_bool, True)
        self.handler_process.start()

##### START WORKER PROCESS #####

    def queue_thread(self, max_size, queue_time, shared_queue): 
        name = "QueueProcess"
        logger.info("Started %r.", name)
        internal_queue = Queue()
        while self.process.value:
            while not shared_queue.empty() and internal_queue.qsize() < max_size:
                internal_queue.put(shared_queue.get())
            now = time.time()
            wait = self.last_time.value + queue_time - now
            while wait > 0 and internal_queue.qsize() < max_size:
                #logger.info("%r sleeping for %f seconds...", name, 0.1)
                time.sleep(0.1)
                now = time.time()
                wait = self.last_time.value + queue_time - now

            logger.info("%r triggered handling of the queue.", name)
            self.handle_queue(internal_queue)

    def handle_queue(self, queue):
        time_diff = time.time() - self.last_time.value
        self.last_time.value = time.time()
        messages = []
        while not queue.empty():
                messages.append(queue.get())

        logger.info("Got a block of %d messages %f seconds since the last run.",
                        len(messages), time_diff)
        if len(messages) > 0:
                logger.info("<Function sendToPdns() would run here on %d messages.>",
                                len(messages))
        #logger.info(messages)

    def prepareForClose(self):
        self.process.value = False
        self.handler_process.join()
        print "Joined child processes in preparation for shutdown."

##### END WORKER PROCESS #####

##### START MAIN PROCESS #####

    def get_consumers(self, Consumer, channel):
        return [Consumer(queues=api_queue,
                         accept=['pickle', 'json'],
                         callbacks=[self.process_api_message]),
                Consumer(queues=monitor_queue,
                         accept=['pickle', 'json'],
                         callbacks=[self.process_monitor_message])
                ]

    def process_api_message(self, body, message):
        self.process_message(body, 'api', message)

    def process_monitor_message(self, body, message):
        self.process_message(body, 'monitor', message)

    def process_message(self, body, key, message):
        #logger.info('Got message: %s', body)
        self.shared_queue.put(body)
        message.ack()
        #if self.shared_queue.qsize() >= self.max_size:
        #        self.handle_queue()

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
            worker = Worker(conn, max_size=10000, queue_time = 5)
            worker.run()
        except KeyboardInterrupt:
            worker.prepareForClose()
            print('Done.')

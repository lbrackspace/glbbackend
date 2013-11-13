from kombu.mixins import ConsumerMixin
from kombu.log import get_logger
from kombu.utils import kwdict, reprcall

from queues import task_queues, api_queue, monitor_queue
from Queue import PriorityQueue
import thread
import time

logger = get_logger(__name__)

pri_map = {
            'api': 10,
            'monitor': 1,
            }

class Worker(ConsumerMixin):

    def __init__(self, connection, max_size=1000, queue_time = 30):
        self.connection = connection
        self.pri_queue = PriorityQueue()
        self.max_size = max_size
        self.last_time = time.time()
        self.queue_time = queue_time
        thread.start_new_thread(self.queue_thread, ("QueueThread",))

    def queue_thread(self, name): 
        logger.info("Started %r.", name)
        while True:
            now = time.time()
            wait = self.last_time + self.queue_time - now
            while wait > 0:
                #logger.info("%r sleeping for %f seconds...", name, wait)
                time.sleep(wait)
                now = time.time()
                wait = self.last_time + self.queue_time - now

            logger.info("%r triggered handling of the queue.", name)
            self.handle_queue()

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
        self.pri_queue.put((pri_map[key], body))
        message.ack()
        if self.pri_queue.qsize() >= self.max_size:
                self.handle_queue()

    def handle_queue(self):
        time_diff = time.time() - self.last_time
        self.last_time = time.time()
        messages = []
        while not self.pri_queue.empty():
                messages.append(self.pri_queue.get(block=True)[1])

        logger.info("Got a block of %d messages %f seconds since the last run.",
                        len(messages), time_diff)
        if len(messages) > 0:
                logger.info("<Function sendToPdns() would run here on %d messages.>",
                                len(messages))
        #logger.info(messages)

if __name__ == '__main__':
    from kombu import Connection, enable_insecure_serializers
    from kombu.utils.debug import setup_logging
    # setup root logger
    setup_logging(loglevel='INFO', loggers=[''])

    enable_insecure_serializers(choices=['pickle'])

    with Connection('amqp://guest:guest@localhost//') as conn:
        try:
            worker = Worker(conn, max_size=10000, queue_time = 5)
            worker.run()
        except KeyboardInterrupt:
            print('Done.')

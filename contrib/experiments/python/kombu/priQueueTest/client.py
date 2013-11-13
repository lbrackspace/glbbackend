from kombu.pools import producers
from random import choice

from queues import task_queues

num_messages = 200000

def send_message(connection, message, routing_key='api'):
    with producers[connection].acquire(block=True) as producer:
        producer.publish(message,
                         serializer='pickle',
                         compression='bzip2',
                         exchange='',
                         declare=task_queues,
                         routing_key=routing_key)

if __name__ == '__main__':
    from kombu import Connection
    types = ['monitor', 'monitor', 'monitor', 'api']

    connection = Connection('amqp://guest:guest@localhost//')
    for m in range(1,num_messages):
        rk = choice(types)
        send_message(connection, message="%r message %d" % (rk,m), routing_key=rk)

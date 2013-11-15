from kombu import Exchange, Queue

#task_exchange = Exchange('tasks', type='direct')
api_queue = Queue('api', '', routing_key='api', durable=True)
#monitor_queue = Queue('monitor', '', routing_key='monitor', durable=True)

task_queues = [api_queue]

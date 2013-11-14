#!/bin/bash

ssh root@client1 priQueueTest/client.py &
ssh root@client2 priQueueTest/client.py &
ssh root@client3 priQueueTest/client.py &
ssh root@client4 priQueueTest/client.py &

#!/bin/bash

#echo "> small file sending test"
valgrind --leak-check=full --show-leak-kinds=all build/task_3/shared_mem/shared_mem $1 $2
echo "> program finished with exit code $?"

md5sum $1
md5sum $2

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes build/task_3/message_queues/message_queues $1 $2
echo "> program finished with exit code $?"

md5sum $1
md5sum $2

CC = gcc
HEADERS = -Iinclude

all:
	@$(CC) $(HEADERS) source/buffer.c source/named_pipes.c -o fifo
	@$(CC) $(HEADERS) source/buffer.c source/shared_memory.c -o shm
	@$(CC) $(HEADERS) source/buffer.c source/message_queue.c -o mqueue

run:
	@python3 run.py

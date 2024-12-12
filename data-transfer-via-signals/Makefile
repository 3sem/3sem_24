CC = gcc

all:
	@$(CC) receiver.c -o receiver
	@$(CC) sender.c -o sender 
	@$(CC) main.c -o main

run:
	@./main output.dat input.dat
	md5sum output.dat input.dat

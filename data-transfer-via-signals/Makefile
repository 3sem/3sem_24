CC = gcc

all:
	@$(CC) receiver.c -o receiver
	@$(CC) sender.c -o sender 
	@$(CC) main.c -o main

run:
	./main input.dat output.dat
	md5sum input.dat output.dat

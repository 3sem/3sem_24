CC = gcc

all:
	@$(CC) reader.c -o reader
	@$(CC) writer.c -o writer

run:
	@./writer input.dat 993
	md5sum input.dat output.dat

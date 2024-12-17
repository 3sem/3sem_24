all:
	gcc -o client -lpthread -fsanitize=address source/client.c source/file_utils.c source/integral.c
	gcc -o server -lpthread -fsanitize=address source/server.c source/file_utils.c source/integral.c

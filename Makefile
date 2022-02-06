all:
	gcc client.d/client.c utils.d/data.c utils.d/numbers.c utils.d/buffer.c utils.d/crypt.c -pthread -o client
	gcc server.d/server.c utils.d/data.c utils.d/numbers.c utils.d/buffer.c utils.d/crypt.c -pthread -o server

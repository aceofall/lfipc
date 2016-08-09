INCLUDE_DIR=./../

default:
	gcc -I${INCLUDE_DIR} -o client client.c
	gcc -I${INCLUDE_DIR} -o server server.c

clean:
	rm -f fifo*
	rm -f client server

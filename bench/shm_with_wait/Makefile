INCLUDE_DIR=./../

default:
	gcc -I${INCLUDE_DIR} -o client client.c -lrt -lpthread
	gcc -I${INCLUDE_DIR} -o server server.c -lrt -lpthread

clean:
	rm -f client server

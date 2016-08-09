INCLUDE_DIR=./../

default:
	gcc -I${INCLUDE_DIR} -o client client.c -lrt
	gcc -I${INCLUDE_DIR} -o server server.c -lrt

clean:
	rm -f test_socket_*
	rm -f client server

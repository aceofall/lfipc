/**
 *	fifo (named pipe) client
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	int msg_size = 0, repeat_count = 0;

	int server, client = -1;
	unsigned int i;
	ssize_t nread;

	/* get param for bench */
	if(get_param(argc, argv, &msg_size, &repeat_count) == -1) {
		return -1;
	}

	/* Make fifo */
	if(mkfifo(FIFO_CLIENT_NAME, PERM_FILE) == -1 && errno != EEXIST) {
		printf("mkfifo error : %s\n", strerror(errno));
		return -1;
	}

	if((server = open(FIFO_SERVER_NAME, O_WRONLY)) < 0) {
		printf("open server error : %s\n", strerror(errno));
		return -1;
	}

	/* send msg */
	measure_start();

	for(i=0; i<repeat_count; i++) {
		write(server, msg, msg_size);

		if(client == -1) {
			client = open(FIFO_CLIENT_NAME, O_RDONLY);
			if(client < 0) {
				printf("open client error : %s\n", strerror(errno));
				break;
			}
		}

		nread = read(client, msg, sizeof(REPLY_MSG));
		if(nread == 0) {
			printf("read size is 0\n");
			break;
		}
	}

	measure_end();

	/* send end msg */
	write(server, END_MSG, sizeof(END_MSG));

	close(client);
	close(server);
	return 0;
}
/**
 *	fifo (named pipe) server
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	int server, client = -1;
	ssize_t nread;

	/* Make fifo */
	if(mkfifo(FIFO_SERVER_NAME, PERM_FILE) == -1 && errno != EEXIST) {
		printf("mkfifo error : %s\n", strerror(errno));
		return -1;
	}

	/* Open fd of fifo */
	if((server = open(FIFO_SERVER_NAME, O_RDONLY)) < 0) {
		printf("open server error : %s\n", strerror(errno));
		return -1;
	}

	while(1) {
		nread = read(server, msg, sizeof(msg));
		if(nread == 0) {
			printf("read size is 0\n");
			break;
		}
		else if(nread == sizeof(END_MSG)) {
			printf("end msg : %s\n", msg);
			break;
		}

		if(client == -1) {
			client = open(FIFO_CLIENT_NAME, O_WRONLY);
		}
		write(client, REPLY_MSG, sizeof(REPLY_MSG));
	}

	close(client);
	close(server);
	return 0;
}

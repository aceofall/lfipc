/**
 *	socket client (connectionless)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	struct sockaddr_un client_addr;
	struct sockaddr_un server_addr;
	int sock;
	ssize_t nread;

	struct sockaddr_storage sa;
	socklen_t sa_len;

	int msg_size, repeat_count;
	unsigned int i;

	/* get param for bench */
	if(get_param(argc, argv, &msg_size, &repeat_count) == -1) {
		return -1;
	}

	/* create socket */
	if((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		printf("socket error : %s\n", strerror(errno));
		return -1;
	}

	strncpy(client_addr.sun_path, SOCKET_CLIENT_NAME, sizeof(SOCKET_CLIENT_NAME));
	client_addr.sun_family = AF_UNIX;
	unlink(SOCKET_CLIENT_NAME);

	if(bind(sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
		printf("bind error : %s\n", strerror(errno));
		goto out;
	}

	/* set server addr */
	strncpy(server_addr.sun_path, SOCKET_SERVER_NAME, sizeof(SOCKET_SERVER_NAME));
	server_addr.sun_family = AF_UNIX;

	/* send msg */
	measure_start();

	for(i=0; i<repeat_count; i++) {
		/* send msg */
		if(sendto(sock, msg, msg_size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
			printf("sendto error : %s\n", strerror(errno));
			break;
		}

		/* read reply */
		nread = read(sock, msg, sizeof(REPLY_MSG));
		if(nread == 0 || (int)nread == -1) {
			printf("read error : %s\n", strerror(errno));
			break;
		}
	}

	measure_end();

	/* send end msg */
	if(sendto(sock, END_MSG, sizeof(END_MSG), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		printf("sendto error : %s\n", strerror(errno));
	}

out:
	/* close socket */
	close(sock);
	unlink(SOCKET_CLIENT_NAME);

	return 0;
}
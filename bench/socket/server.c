/**
 *	socket server (connectionless)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	struct sockaddr_un server_addr;
	int sock;
	ssize_t nread;

	struct sockaddr_storage sa;
	socklen_t sa_len;

	/* create socket */
	if((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		printf("socket error : %s\n", strerror(errno));
		return -1;
	}

	strncpy(server_addr.sun_path, SOCKET_SERVER_NAME, sizeof(SOCKET_SERVER_NAME));
	server_addr.sun_family = AF_UNIX;
	unlink(SOCKET_SERVER_NAME);

	if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		printf("bind error : %s\n", strerror(errno));
		goto out;
	}

	while(1) {
		/* read msg */
		sa_len = sizeof(sa);
		nread = recvfrom(sock, msg, sizeof(msg), 0, (struct sockaddr*)&sa, &sa_len);
		if(nread == 0 || (int)nread == -1) {
			printf("recvfrom error : %s\n", strerror(errno));
			break;
		}
		else if(nread == sizeof(END_MSG)) {
			printf("end msg : %s\n", msg);
			break;
		}

		/* prepare msg */
		strncpy(msg, REPLY_MSG, sizeof(REPLY_MSG));

		/* send reply */
		if(sendto(sock, msg, sizeof(REPLY_MSG), 0, (struct sockaddr*)&sa, sa_len) == -1) {
			printf("sendto error : %s\n", strerror(errno));
			break;
		}
	}

out:
	/* close socket */
	close(sock);
	unlink(SOCKET_SERVER_NAME);

	return 0;
}

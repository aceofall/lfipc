/**
 *	msg queue server (POSIX)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	struct mqueue_msg msg;
	struct mq_attr attr = {
		.mq_flags = O_CREAT | O_RDWR,
		.mq_maxmsg = 100,
		.mq_msgsize = MQUEUE_MAX_MSG_SIZE,
	};
	mqd_t id;
	ssize_t nread;

	/* create mqueue */
	if((id = mq_open(MQUEUE_NAME, O_CREAT | O_RDWR, PERM_FILE, NULL)) == -1) {
		printf("mq_open error : %s\n", strerror(errno));
		return -1;
	}

	while(1) {
		nread = mq_receive(id, (char*)&msg, sizeof(msg), NULL);
		if((int)nread == -1) {
			printf("mq_receive error : %s\n", strerror(errno));
			break;
		}
		else if(nread == (sizeof(END_MSG) + sizeof(msg.type) + sizeof(msg.id))) {
			printf("end msg : %s\n", msg.msg);
			break;
		}

		/* prepare msg */
		strncpy(msg.msg, REPLY_MSG, sizeof(REPLY_MSG));

		/* send reply */
		if(mq_send(msg.id, (const char*)&msg, (sizeof(msg.type) + sizeof(msg.id) + sizeof(REPLY_MSG)), 0) == -1) {
			printf("mq_send error : %s\n", strerror(errno));
			break;
		}
	}

	/* destroy mqueue */
	if(mq_close(id) == -1) {
		printf("mq_close error : %s\n", strerror(errno));
		return -1;
	}

	if(mq_unlink(MQUEUE_NAME) == -1) {
		printf("mq_unlink error : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

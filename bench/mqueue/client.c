/**
 *	msg queue client (POSIX)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	struct mqueue_msg msg;
	mqd_t id;
	ssize_t nread;

	int msg_size, repeat_count;
	unsigned int i;

	/* get param for bench */
	if(get_param(argc, argv, &msg_size, &repeat_count) == -1) {
		return -1;
	}

	/* get mqueue */
	if((id = mq_open(MQUEUE_NAME, O_RDWR)) == -1) {
		printf("msgget error : %s\n", strerror(errno));
		return -1;
	}

	/* send msg */
	measure_start();

	for(i=0; i<repeat_count; i++) {
		/* prepare msg */
		msg.type = 1;
		msg.id = id;
		
		/* send msg */
		if(mq_send(msg.id, (const char*)&msg, (sizeof(msg.type) + sizeof(msg.id) + msg_size), 0) == -1) {
			printf("mq_send error : %s\n", strerror(errno));
			break;
		}

		/* read reply */
		nread = mq_receive(id, (char*)&msg, sizeof(msg), NULL);
		if((int)nread == -1) {
			printf("mq_receive error : %s\n", strerror(errno));
			break;
		}
	}

	measure_end();

	/* send end msg */
	msg.type = 1;
	msg.id = id;
	strncpy(msg.msg, END_MSG, sizeof(END_MSG));

	if(mq_send(msg.id, (const char*)&msg, (sizeof(msg.type) + sizeof(msg.id) + sizeof(END_MSG)), 0) == -1) {
		printf("mq_send error : %s\n", strerror(errno));
	}

	/* close mqueue */
	if(mq_close(id) == -1) {
		printf("mq_close error : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}
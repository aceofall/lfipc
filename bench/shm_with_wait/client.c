/**
 *	shared memory client with wait (POSIX)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	struct shm_msg *client_msg;
	struct shm_msg *server_msg;
	struct shm_mutex_cond *smc;

	void *addr;
	int fd;
	ssize_t nread;

	int msg_size, repeat_count;
	unsigned int i;

	/* get param for bench */
	if(get_param(argc, argv, &msg_size, &repeat_count) == -1) {
		return -1;
	}

	/* get shm */
	if((fd = shm_open(SHM_NAME, O_RDWR, PERM_FILE)) == -1) {
		printf("shm_open error : %s\n", strerror(errno));
		return -1;
	}

	/* mmap */
	addr = mmap(NULL, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
		printf("mmap error : %s\n", strerror(errno));
		goto out;
	}
	smc = (struct shm_mutex_cond*)addr;

	client_msg = (struct shm_msg *)((char*)addr + sizeof(struct shm_mutex_cond) + SHM_CLIENT_BUF_IDX);
	server_msg = (struct shm_msg *)((char*)addr + sizeof(struct shm_mutex_cond) + SHM_SERVER_BUF_IDX);

	/* send msg */
	measure_start();

	for(i=0; i<repeat_count; i++) {
		/* prepare msg */
		client_msg->status = 0;
		client_msg->len = msg_size;
		
		/* send msg */
		memcpy(client_msg->msg, msg, client_msg->len);
		client_msg->status = 1;

		/* send a signal to notify there is a msg */
		shm_send_signal(smc);

		/* read reply */
		shm_wait_signal(smc);
	}

	measure_end();

	/* send end msg */
	client_msg->status = 0;
	client_msg->len = sizeof(END_MSG);
	strncpy(client_msg->msg, END_MSG, client_msg->len);
	client_msg->status = 1;

	shm_send_signal(smc);

out:
	/* close shm */
	if(munmap(addr, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond)) == -1) {
		printf("munmap error : %s\n", strerror(errno));
	}

	if(close(fd) == -1) {
		printf("close error : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

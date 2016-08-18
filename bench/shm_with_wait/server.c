/**
 *	shared memory server with wait (POSIX)
 *
 *	Copyright (C) 2016  Jinbum Park <jinb.park7@gmail.com>
*/

#include "common.h"

/* Helper for mutex, cond */
int mutex_cond_init(void *addr)
{
	int ret;
	struct shm_mutex_cond *smc = (struct shm_mutex_cond *)addr;

	ret = pthread_mutexattr_init(&smc->mutex_attr);
	if(ret) {
		printf("pthread_attr_init error\n");
		return -1;
	}

	ret = pthread_mutexattr_setpshared(&smc->mutex_attr, PTHREAD_PROCESS_SHARED);
	if(ret) {
		printf("pthread_mutexattr_setpshared error\n");
		return -1;
	}

	ret = pthread_condattr_init(&smc->cond_attr);
	if(ret) {
		printf("pthread_condattr_init error\n");
		return -1;
	}

	ret = pthread_condattr_setpshared(&smc->cond_attr, PTHREAD_PROCESS_SHARED);
	if(ret) {
		printf("pthread_condattr_setpshared error\n");
		return -1;
	}

	ret = pthread_mutex_init(&smc->mutex, &smc->mutex_attr);
	if(ret) {
		printf("pthread_mutex_init error\n");
		return -1;
	}

	ret = pthread_cond_init(&smc->cond, &smc->cond_attr);
	if(ret) {
		printf("pthread_cond_init error\n");
		return -1;
	}

	return 0;
}

void mutex_cond_destroy(void *addr)
{
	struct shm_mutex_cond *smc = (struct shm_mutex_cond *)addr;

	pthread_condattr_destroy(&smc->cond_attr);
	pthread_mutexattr_destroy(&smc->mutex_attr);
	
	pthread_cond_destroy(&smc->cond);
	pthread_mutex_destroy(&smc->mutex);
}

/* Server */
int main(int argc, char **argv)
{
	char msg[MSG_SIZE_MAX];
	struct shm_msg *client_msg;
	struct shm_msg *server_msg;
	struct shm_mutex_cond *smc;

	void *addr;
	int fd;
	ssize_t nread;
	pid_t pid;

	/* Parent is Server */

	/* create shm */
	if((fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, PERM_FILE)) == -1) {
		printf("shm_open error : %s\n", strerror(errno));
		return -1;
	}

	/* set size */
	if(ftruncate(fd, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond)) == -1) {
		printf("ftruncate error : %s\n", strerror(errno));
		goto out;
	}

	/* mmap */
	addr = mmap(NULL, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
		printf("mmap error : %s\n", strerror(errno));
		goto out;
	}
	memset(addr, 0, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond));

	/* Init mutex, cond */
	if(mutex_cond_init(addr)) {
		printf("mutex_cond_init error\n");
		return -1;
	}
	smc = (struct shm_mutex_cond*)addr;

	client_msg = (struct shm_msg *)((char*)addr + sizeof(struct shm_mutex_cond) + SHM_CLIENT_BUF_IDX);
	server_msg = (struct shm_msg *)((char*)addr + sizeof(struct shm_mutex_cond) + SHM_SERVER_BUF_IDX);

	while(1) {
		/* read msg */
		shm_wait_signal(smc);

		if(client_msg->len == sizeof(END_MSG)) {
			printf("end msg : %s\n", client_msg->msg);
			break;
		}

		/* prepare msg */
		server_msg->status = 0;
		server_msg->len = sizeof(REPLY_MSG);

		/* send reply */
		strncpy(server_msg->msg, REPLY_MSG, sizeof(REPLY_MSG));
		server_msg->status = 1;

		/* send a signal to notify there is a msg */
		shm_send_signal(smc);
	}

out:
	mutex_cond_destroy(addr);

	/* destroy shm */
	if(munmap(addr, MSG_SIZE_MAX + sizeof(struct shm_mutex_cond)) == -1) {
		printf("munmap error : %s\n", strerror(errno));
	}

	if(close(fd) == -1) {
		printf("close error : %s\n", strerror(errno));
	}

	if(shm_unlink(SHM_NAME) == -1) {
		printf("shm_unlink error : %s\n", strerror(errno));
	}

	return 0;
}

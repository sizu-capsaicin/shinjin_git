#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>

#define UNIX_PATH "./tmp/unix-domain-path"	// define the unix domain path
#define BUF 256					// define the buffer size
#define QUE 128					// define the que size

int main(void)
{
	int desc;	// use descripter

	if ((desc = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("socket error. errno: %d\n", errno);
		exit(1);
	}

	struct sockaddr_un addr;	// use sockaddr
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, UNIX_PATH);

	// if the file already exist, remove it
	remove(UNIX_PATH);

	if ((bind(desc, (struct sockaddr *) &addr, sizeof(struct sockaddr_un))) < 0) {
		printf("bind error. errno: %d\n", errno);
		exit(1);
	}

	if (listen(desc, QUE) < 0) {
		printf("listen error. errno: %d\n", errno);
		exit(1);
	}

	int fd;		// use descripter for new socket maked by accept()

	if ((fd = accept(desc, NULL, NULL)) < 0) {	// why NULL ?
		printf("accept error. errno: %d\n", errno);
		exit(1);
	}

	char buf[BUF];			// receive the data
	// initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {			// execute forever
		int size;		// size of data

		if ((size = recv(fd, buf, BUF, 0)) < 0) {
			printf("recv error. errno: %d\n", errno);
			exit(1);
		}

		buf[size - 1] = '\0';
		// print the data
		printf("receive: %s\n", buf);
		if (!strncmp(buf, "exit\0", BUF)) {
			break;
		} else {
			if (send(fd, buf, BUF, 0) < 0) {
				printf("send error. errno: %d\n", errno);
				exit(1);
			}
		}
	}

	close(fd);
	close(desc);

	return 0;
}


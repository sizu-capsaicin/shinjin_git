#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>

#define UNIX_PATH "./tmp/unix-domain-path"	// define unix domain path
#define BUF 256					// define buffer size

int main(void)
{
	int dscr;	// use descripter

	if ((dscr = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("socket error. errno: %d\n", errno);
		exit(1);
	}

	struct sockaddr_un addr;	// use sockaddr
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, UNIX_PATH);

	if (connect(dscr, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) < 0) {
		printf("connect error. errno: %d\n", errno);
		exit(1);
	}

	char buf[BUF];		//receive input and send it
	//initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {
		printf("> ");
		if (fgets(buf, BUF, stdin) == NULL) {	//scan stdin
			printf("fgets error.");
			exit(1);
		} else {
			int size = strlen(buf);
			buf[size - 1] = '\0';

			if (send(dscr, buf, BUF, 0) < 0) {		// send data
				printf("send error. errno: %d\n", errno);
				exit(1);
			}
			
			if (!strncmp(buf, "exit\0", BUF)) {
				break;
			}

			if ((size = recv(dscr, buf, BUF, 0)) < 0) {
				printf("recv error. errno: %d^n", errno);
				exit(1);
			} else {
				buf[size - 1] = '\0';
				// print data
				printf("echo: %s\n", buf);
			}
		}
	}

	close(dscr);	// close the socket

	return 0;
}

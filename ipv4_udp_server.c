#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF 256					// define the buffer size
#define QUE 128					// define the que size

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Useage: ./<exec filename> <port number of server>\n");
		exit(1);
	}

	int desc;	// use descripter

	if ((desc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket error\n");
		exit(1);
	}

	struct sockaddr_in server, client;	// use sockaddr
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = atoi(argv[1]);

	if ((bind(desc, (struct sockaddr *) &server, sizeof(struct sockaddr_in))) < 0) {
		perror("bind error\n");
		exit(1);
	}

	int client_len = sizeof(client);
	char buf[BUF];			// receive the data

	// initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {			// execute forever
		int size;		// size of data

		if ((size = recvfrom(desc, buf, BUF, 0, (struct sockaddr *) &client, (socklen_t *) &client_len)) < 0) {
			perror("recv error\n");
			exit(1);
		}

		buf[size - 1] = '\0';
		// print the data
		printf("receive: %s\n", buf);
		if (!strncmp(buf, "exit\0", BUF)) {
			break;
		} else {
			if (sendto(desc, buf, BUF, 0, (struct sockaddr *) &client, (socklen_t) client_len) < 0) {
				perror("send error\n");
				exit(1);
			}
		}
	}

	close(desc);

	return 0;
}


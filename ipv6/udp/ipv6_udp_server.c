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
#include <netdb.h>

#define BUF 256					// define the buffer size
#define QUE 128					// define the que size

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Useage: ./<exec filename> <ip version number> <port number of server>\n");
		exit(1);
	}

	int desc;	// use descripter
  int domain; // house domain

  if (!(strncmp("6", argv[1], sizeof("6"))))
    domain = AF_INET6;
  else 
    domain = AF_INET;

	struct addrinfo server, client, *res;	// use sockaddr
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	server.ai_family = domain;
	server.ai_socktype = SOCK_DGRAM;
  server.ai_flags = AI_PASSIVE;

  int err = 0;
  err = getaddrinfo(NULL, argv[2], &server, &res);
  if (err != 0) {
	  printf("getaddrinfo error: %s\n", gai_strerror(err));
    exit(1);
  }

	if ((desc = socket(res->ai_family, res->ai_socktype, 0)) < 0) {
		perror("socket error\n");
		exit(1);
	}

	if ((bind(desc, res->ai_addr, res->ai_addrlen)) < 0) {
		perror("bind error\n");
		exit(1);
	}

  printf("ip version: IPv%s\n", argv[1]);

	client.ai_addrlen = sizeof(client.ai_addr);
	char buf[BUF];			// receive the data

	// initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {			// execute forever
		int size;		// size of data

		if ((size = recvfrom(desc, buf, BUF, 0, (struct sockaddr *) client.ai_addr, &client.ai_addrlen)) < 0) {
			perror("recv error\n");
			exit(1);
		}

		buf[size - 1] = '\0';
		// print the data
		printf("receive: %s\n", buf);
		if (!strncmp(buf, "exit\0", BUF)) {
			break;
		} else {
			if (sendto(desc, buf, BUF, 0, (struct sockaddr *) client.ai_addr, client.ai_addrlen) < 0) {
				perror("send error\n");
				exit(1);
			}
		}
	}

	close(desc);

	return 0;
}


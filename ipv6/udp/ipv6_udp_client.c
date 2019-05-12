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

#define BUF 256					// define buffer size

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Useage: ./<exec filename> <dest ip address> <port number>\n");
		exit(1);
	}

	int dscr;	// use descripter
	struct addrinfo addr, *res;	// use sockaddr

  memset(&addr, 0, sizeof(addr));
	addr.ai_family = AF_UNSPEC;
  addr.ai_socktype = SOCK_DGRAM;

  int err = 0;
  err = getaddrinfo(argv[1], argv[2], &addr, &res);
  if (err != 0) {
    printf("getaddrinfo error: %s\n", gai_strerror(err));
    exit(1);
  }

	if ((dscr = socket(res->ai_family, res->ai_socktype, 0)) < 0) {
		perror("socket error\n");
		exit(1);
	}

	char buf[BUF];		//receive input and send it
	int addr_len = sizeof(addr);

	//initiation
  memset(buf, 0, BUF);

	while (1) {
		printf("> ");
		if (fgets(buf, BUF, stdin) == NULL) {	//scan stdin
			perror("fgets error\n");
			exit(1);
		} else {
			int size = strlen(buf);
			buf[size - 1] = '\0';

      const char *ipver;
      switch (res->ai_family) {
        case AF_INET:
          ipver = "IPv4";
          break;
        case AF_INET6:
          ipver = "IPv6";
          break;
        default:
          ipver = "unknown";
          break;
      }
      printf("IP version: %s\n", ipver);

			if (sendto(dscr, buf, BUF, 0, res->ai_addr, res->ai_addrlen) < 0) {		// send data
				perror("send error\n");
				exit(1);
			}
			
			if (!strncmp(buf, "exit\0", BUF)) {
				break;
			}

			if ((size = recvfrom(dscr, buf, BUF, 0, res->ai_addr, &res->ai_addrlen)) < 0) {
				perror("recv error\n");
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

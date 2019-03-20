#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define UNIX_PATH "./tmp/unix-domain-path"	// define the unix domain path
#define BUF 256					// define the buffer size
#define QUE 128					// define the que size
#define INTERFACE "ens160"

int main(void)
{
	int desc;	// use descripter

	if ((desc = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("socket error. errno: %d\n", errno);
		exit(1);
	}

	struct ifreq ifr;		// set index number of interfacue
	int interface_index;		// set number of interface index
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, INTERFACE, IFNAMSIZ);
	ioctl(desc, SIOCGIFINDEX, &ifr);
	interface_index = ifr.ifr_ifindex;

	struct sockaddr_ll addr;	// use sockaddr
	bzero(&addr, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = interface_index;

	if ((bind(desc, (struct sockaddr *) &addr, sizeof(struct sockaddr_ll))) < 0) {
		printf("bind error. errno: %d\n", errno);
		exit(1);
	}

	char buf[BUF];			// receive the data
	// initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {
		fd_set fds;
		struct timeval t;
		int num;
		// set the set of file descripter
		FD_ZERO(&fds);
		FD_SET(desc, &fds);
		memset(&t, 0, sizeof(t));
		// set the number of file descripter
		num = select(FD_SETSIZE, &fds, NULL, NULL, &t);
		if (num > 0) {
			int size;		// size of data

			if ((size = recv(desc, buf, BUF, 0)) < 0) {
				printf("recv error. errno: %d\n", errno);
				exit(1);
			}

			buf[size - 1] = '\0';
			// print the data
			int line = 0;
			printf("packet start");
			for (int i = 0; i < size; i++) {
				if (!(i % 16)) {
					printf("\n0x%06x: ", line);
					line++;
				}
				printf("%02x", buf[i]);
				if (i % 2)
					printf(" ");
			}
			printf("\n\n");
		}
	}

	close(desc);

	return 0;
}


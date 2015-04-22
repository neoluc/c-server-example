#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#define MAX_THREAD_NUMBER 10

int main(int argc, char **argv) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket()");
		exit(1);
	}
	in_addr_t ip = inet_addr("127.0.0.1");
	if (ip < 0) {
		perror("inet_addr()");
		exit(1);
	}
	unsigned short port = 12345;
	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ip;
	address.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0) {
		perror("inet_addr()");
		exit(1);
	}

	while (1) {
		char buffer[512] = {0};
		sprintf(buffer, "%d", rand());
		if (send(fd, &buffer, sizeof(buffer), 0) < 0) {
			perror("send()");
			exit(1);
		}
		char receive_buffer[512];
		if (recv(fd, receive_buffer, sizeof(receive_buffer), 0) < 0) {
			perror("recv()");
			exit(1);
		}
		printf("%s\n", receive_buffer);
		sleep(1);
	}
	return 0;
}
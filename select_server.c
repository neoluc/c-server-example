#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

int main(int argc, char **argv) {

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("socket()");
		exit(1);
	}

	unsigned short port = 12345;
	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
		perror("bind()");
		exit(1);
	}

	if (listen(server_fd, 1024) != 0) {
		perror("bind()");
		exit(1);
	}

	fd_set sets;
	FD_ZERO(&sets);
	FD_SET(server_fd, &sets);
	int max_fd = server_fd;

	while (1) {
		fd_set read_fds = sets;
		int select_result = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (select_result < 0) {
			perror("select()");
			exit(1);
		}
		if (select_result == 0) {
			printf("select time out\n");
			continue;
		}
		for (int fd = 0; fd <= max_fd; fd++) {
			if (FD_ISSET(fd, &read_fds)) {
				if (fd == server_fd) {
					struct sockaddr_in client_address;
					unsigned int client_address_length = sizeof(struct sockaddr_in);
					int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_length);
					if (client_fd < 0) {
						perror("accept()");
						exit(1);
					}
					FD_SET(client_fd, &sets);		
					if (client_fd > max_fd) {
						max_fd = client_fd;
					}
				} else {
					char buffer[512] = {0};
					int received_size = recv(fd, buffer, sizeof(buffer), 0);
					if (received_size <= 0) {
						perror("recv()");
						if (close(fd) < 0) {
							perror("close()");
							exit(1);
						}
						FD_CLR(fd, &sets);
					} else {
						printf("%s\n", buffer);
						if (send(fd, buffer, received_size, 0) < 0) {
							perror("send()");
							exit(1);
						}
					}
				}
			}
		}
	}
}
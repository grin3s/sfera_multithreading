/*
 * chat_client.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: grin
 */
#include "chat_client.h"
#include "chat_utils.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <system_error>
#include <iostream>

#define ERR_CHECK(f) if ((f) == -1) throw std::system_error(errno, std::system_category());

int ChatClient::run() {
	//	fd_set rfds, master;
	//	int retval;
	//
	//	try {
	//		FD_ZERO(&master);
	//		//stdin
	//		FD_SET(0, &master);
	//
	//		ERR_CHECK(sock_fd = socket(PF_INET, SOCK_STREAM, 0));
	//
	//		ERR_CHECK(make_socket_non_blocking(sock_fd));
	//		FD_SET(sock_fd, &master);
	//
	//		struct sockaddr_in sockAddr;
	//		sockAddr.sin_family = AF_INET;
	//		sockAddr.sin_port = htons(3100);
	//		sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	//		socklen_t len = sizeof(sockAddr);
	//		connect(sock_fd, (const sockaddr*) &sockAddr, len);
	//
	//		while (1) {
	//			rfds = master;
	//			retval = select(sock_fd + 1, &rfds, NULL, NULL, NULL);
	//
	//			if (FD_ISSET(0, &rfds)) {
	//				while (1) {
	//					int n_read;
	//					n_read = read(0, buf, BUF_SIZE);
	//					if (n_read > 0) {
	//						write(0, "haha\n", 5);
	//						send(sock_fd, buf, n_read, MSG_NOSIGNAL);
	//					}
	//					else
	//						break;
	//				}
	//			}
	//			else if (FD_ISSET(sock_fd, &rfds)) {
	//				while (1) {
	//					int n_read;
	//					n_read = recv(0, buf, BUF_SIZE, 0);
	//					if (n_read > 0)
	//						write(0, buf, n_read);
	//					else
	//						break;
	//				}
	//			}
	//		}
	//		return 0;
	//	}
	//	catch (const std::system_error& e) {
	//		std::cout << "Caught system_error with code " << e.code() << " meaning " << e.what() << '\n';
	//		return -1;
	//	}

	fd_set readfds;
	struct sockaddr_in serv_addr;
	int main_sock;
	char buf[BUF_SIZE];

	if ((main_sock = socket(AF_INET, SOCK_STREAM,0)) == -1) {
		printf("Error: socket\n");
		return -1;
	}

	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(3100);
	if (connect(main_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
		printf("Error: connect\n");
		return -1;
	}
	make_socket_non_blocking(main_sock);

	for (;;) {
		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(main_sock, &readfds);
		if (select(main_sock + 1, &readfds, NULL, NULL, NULL)==-1) {
			printf("Error: select\n");
			return -1;
		}

		if (FD_ISSET(0, &readfds)) {
			int n_read;
			n_read = read(0, buf, BUF_SIZE);
			if (n_read > 0) {
				if (send(main_sock, buf, n_read, MSG_NOSIGNAL) == -1) {
					printf("Error: send\n");
					return -1;
				}
			}

		}
		if (FD_ISSET(main_sock, &readfds)) {
			int n_read = recv(main_sock, buf, BUF_SIZE, 0);
			if (n_read == 0) {
				shutdown(main_sock, SHUT_RDWR);
				close(main_sock);
				break;
			}
			else if (n_read == -1) {
				printf("Error: recv\n");
				return -1;
			}
			write(0, buf, n_read);
		}
	}
	return 0;
}





/*
 * chat_server.h
 *
 *  Created on: Mar 23, 2016
 *      Author: grin
 */

#ifndef P2_CHAT_SERVER_H_
#define P2_CHAT_SERVER_H_
#include <unordered_map>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <system_error>
#include <errno.h>
#include <unistd.h>
#include "client_buffer.h"

#define MAXEVENTS 64
#define MAX_MESSAGE_SIZE 5
#define BUF_SIZE MAX_MESSAGE_SIZE

class ChatServer {
	int listener_fd;
	int epoll_fd;
	struct sockaddr_in my_addr;
	struct epoll_event event;
	struct epoll_event *events = nullptr;
	char buf[BUF_SIZE];
	std::unordered_map<int, ClientBuffer> clients;

	void close_socket(int sock_fd);
	int make_socket_non_blocking (int sfd);

	void process_input_buffer(int sock_fd, int main_buf_len);
	void send_to_all(char *msg, int len);

public:
	int run(short port);
};



#endif /* P2_CHAT_SERVER_H_ */

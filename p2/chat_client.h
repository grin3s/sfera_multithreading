/*
 * chat_client.h
 *
 *  Created on: Mar 25, 2016
 *      Author: grin
 */

#ifndef P2_CHAT_CLIENT_H_
#define P2_CHAT_CLIENT_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 1024

class ChatClient {
	int sock_fd;
	char buf[BUF_SIZE];
public:
	int run();
};




#endif /* P2_CHAT_CLIENT_H_ */

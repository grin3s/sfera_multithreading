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
#include <unordered_map>
#include "chat_server.h"

#define PORT 3100

int main(int argc, char **argv) {
	ChatServer().run(PORT);
	return 0;
}

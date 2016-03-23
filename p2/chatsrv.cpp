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
#include <unordered_set>

#define PORT 3100
#define MAXEVENTS 64
#define MAX_MESSAGE_SIZE 1024

#define ERR_CHECK(f) if ((f) == -1) throw std::system_error(errno, std::system_category());

void close_socket(int sock_fd) {
	printf("connection terminated\n");
	shutdown(sock_fd, SHUT_RDWR);
	close(sock_fd);
}

int make_socket_non_blocking (int sfd)
{
	int flags, s;

	if ((flags = fcntl (sfd, F_GETFL, 0)) == -1)
	{
		return -1;
	}

	flags |= O_NONBLOCK;
	if ((s = fcntl (sfd, F_SETFL, flags)) == -1)
	{
		return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
	int listener_fd;
	int epoll_fd;
	struct sockaddr_in my_addr;
	struct epoll_event event;
	struct epoll_event *events = nullptr;
	char buf[MAX_MESSAGE_SIZE];
	std::unordered_set<int> clients;

	try {
		ERR_CHECK(listener_fd = socket(PF_INET, SOCK_STREAM, 0));

		bzero(&my_addr, sizeof(struct sockaddr_in));
		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(PORT);
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		ERR_CHECK(bind(listener_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)));

		ERR_CHECK(make_socket_non_blocking(listener_fd));

		ERR_CHECK(listen(listener_fd, SOMAXCONN));

		int yes = 1;

		ERR_CHECK(setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

		ERR_CHECK(epoll_fd = epoll_create1(0));

		event.data.fd = listener_fd;
		event.events = EPOLLIN | EPOLLET;
		ERR_CHECK(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener_fd, &event));

		events = (epoll_event *)calloc(MAXEVENTS, sizeof(event));

		while (1) {
			int n_events;
			n_events = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
			for (int i = 0; i < n_events; i++) {
				if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
					//throw std::system_error(errno, std::system_category());
					close_socket(events[i].data.fd);
					clients.erase(events[i].data.fd);
				}
				else if (listener_fd == events[i].data.fd) {
					while (1) {
						struct sockaddr new_addr;
						socklen_t in_len = sizeof(new_addr);
						int new_fd;
						if ((new_fd = accept(listener_fd, &new_addr, &in_len)) == -1) {
							if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
								// we have no new connections
								break;
							}
							else {
								throw std::system_error(errno, std::system_category());
							}
						}
						printf("accepted connection\n");
						ERR_CHECK(make_socket_non_blocking(new_fd));

						// adding new desciptor to epoll
						event.data.fd = new_fd;
						event.events = EPOLLIN | EPOLLET;
						ERR_CHECK(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event));
						clients.insert(new_fd);
						ERR_CHECK(send(new_fd, "Welcome\n", 8, MSG_NOSIGNAL));
					}
				}
				else {
					// we read from some socket
					//printf("reading from socket\n");
					ssize_t n_read;
					bool close_conn = false;
					// reading one less, so that we
					while (1) {
						n_read = recv(events[i].data.fd, buf, MAX_MESSAGE_SIZE - 1, 0);
						if (n_read == -1) {
							if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
								close_conn = true;
							}
							// or else everything is ok, we just read everything we could
							break;
						}
						else if (n_read == 0) {
							// connection is closed
							close_conn = true;
							break;
						}
						write(1, buf, n_read);
						for (auto client_fd = clients.begin(); client_fd != clients.end(); client_fd++) {
							ERR_CHECK(send(*client_fd, buf, n_read, MSG_NOSIGNAL));
						}
					}
					if (close_conn) {
						clients.erase(events[i].data.fd);
						close_socket(events[i].data.fd);
					}
				}
			}
		}
	}
	catch (const std::system_error& e) {
		std::cout << "Caught system_error with code " << e.code() << " meaning " << e.what() << '\n';
		if (events != nullptr) {
			free(events);
		}
		return -1;
	}
	if (events != nullptr) {
		free(events);
	}
	return 0;
}

/*
 * chat_server.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: grin
 */
#include "chat_server.h"
#include <iostream>
#include "chat_utils.h"


#define ERR_CHECK(f) if ((f) == -1) throw std::system_error(errno, std::system_category());

int ChatServer::run(short port) {
	try {
		ERR_CHECK(listener_fd = socket(PF_INET, SOCK_STREAM, 0));

		bzero(&my_addr, sizeof(struct sockaddr_in));
		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(port);
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		int yes = 1;
		ERR_CHECK(setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)));

		ERR_CHECK(bind(listener_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)));

		ERR_CHECK(make_socket_non_blocking(listener_fd));

		ERR_CHECK(listen(listener_fd, SOMAXCONN));



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
						clients.insert(std::pair<int, ClientBuffer>(new_fd, ClientBuffer()));
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
						n_read = recv(events[i].data.fd, buf, BUF_SIZE, 0);
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
						process_input_buffer(events[i].data.fd, n_read);
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

void ChatServer::close_socket(int sock_fd) {
	printf("connection terminated\n");
	shutdown(sock_fd, SHUT_RDWR);
	close(sock_fd);
}

void ChatServer::process_input_buffer(int sock_fd, int main_buf_len) {
	//std::cout << "process_input_buffer" << std::endl;
	ClientBuffer &client_buf = clients.find(sock_fd)->second;
	int i = 0;
	bool found = false;
	//std::cout << "1 " << client_buf;

	//find first message, we have to merge it with the one in the client buffer
	auto first_len = client_buf.get_size();
	while ((first_len < MAX_MESSAGE_SIZE) && (i < main_buf_len)) {
		if (buf[i] == '\n') {
			found = true;
			break;
		}
		i++;
		first_len++;
	}
	if (found) {
		client_buf.append(buf, i + 1);
		send_to_all(client_buf.get_data(), client_buf.get_size());
		client_buf.flush();
		i++;
	}
	else {
		if (first_len == MAX_MESSAGE_SIZE) {
			client_buf.append(buf, i - 1);
			client_buf.append('\n');
			send_to_all(client_buf.get_data(), client_buf.get_size());
			client_buf.flush();
			i--;
		}
	}

	//std::cout << "2 " << client_buf;
	auto j = main_buf_len - 1;
	found = false;
	while (j >= i) {
		if (buf[j] == '\n') {
			found = true;
			break;
		}
		j--;
	}

	if (found) {
		send_to_all(buf + i, j - i + 1);
		j++;
	}
	else {
		j = i;
	}

	if (j < main_buf_len) {
		client_buf.append(buf + j, main_buf_len - j);
	}
	//std::cout << "3 " << client_buf;
}

void ChatServer::send_to_all(char *msg, int len) {
	for (auto client_fd = clients.begin(); client_fd != clients.end(); client_fd++) {
		ERR_CHECK(send(client_fd->first, msg, len, MSG_NOSIGNAL));
	}
}





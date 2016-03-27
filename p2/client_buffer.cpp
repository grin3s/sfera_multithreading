/*
 * client_buffer.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: grin
 */
#include "client_buffer.h"
#include <iostream>

void ClientBuffer::append(char *data, int n_chars) {
//	std::cout << buf.capacity() << std::endl;
//	std::cout << "inserting" << std::endl;
	buf.insert(buf.end(), data, data + n_chars);
}

void ClientBuffer::append(char c) {
//	std::cout << "pushing" << std::endl;
	buf.push_back(c);
}

void ClientBuffer::flush() {
//	std::cout << "flushing" << std::endl;
	buf.clear();
}
char *ClientBuffer::get_data() {
	return buf.data();
}

size_t ClientBuffer::get_size() {
	return buf.size();
}

bool ClientBuffer::is_empty() {
	return buf.empty();
}

std::ostream &operator<<(std::ostream &out, const ClientBuffer& client_buf) {
	for (auto iter = client_buf.buf.begin(); iter != client_buf.buf.end(); iter++) {
		out << *iter;
	}
	return out;
}

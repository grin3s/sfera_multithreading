/*
 * client_buffer.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: grin
 */
#include "client_buffer.h"

void ClientBuffer::append(char *data, int n_chars) {
	buf.insert(buf.end(), data, data + n_chars);
}

void ClientBuffer::append(char c) {
	buf.push_back(c);
}

void ClientBuffer::flush() {
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



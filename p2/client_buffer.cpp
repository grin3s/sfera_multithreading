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

int ClientBuffer::get_size() {
	return buf.size();
}





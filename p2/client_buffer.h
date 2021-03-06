/*
 * client_buffer.h
 *
 *  Created on: Mar 23, 2016
 *      Author: grin
 */

#ifndef P2_CLIENT_BUFFER_H_
#define P2_CLIENT_BUFFER_H_

#include <vector>
#include <iostream>

class ClientBuffer {
	std::vector<char> buf;
public:
	void append(char *data, int n_chars);
	void append(char c);
	void flush();
	char *get_data();
	size_t get_size();
	bool is_empty();
	friend std::ostream &operator<<(std::ostream &out, const ClientBuffer& client_buf);
};

std::ostream &operator<<(std::ostream &out, const ClientBuffer& client_buf);




#endif /* P2_CLIENT_BUFFER_H_ */

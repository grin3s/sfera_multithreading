/*
 * chat_utils.h
 *
 *  Created on: Mar 25, 2016
 *      Author: grin
 */

#ifndef P2_CHAT_UTILS_H_
#define P2_CHAT_UTILS_H_
#include <fcntl.h>

int make_socket_non_blocking (int sfd) {
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



#endif /* P2_CHAT_UTILS_H_ */

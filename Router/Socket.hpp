#pragma once

#include <sstream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#include "Configure.hpp"

namespace ws {

	class Socket {
	
	public:
		typedef std::vector<std::pair<int, int> >	socket_type;

	private:
		std::vector<sockaddr_in> _server_addr;

		/*
			first: server socket fd
			second: server kqueue fd
		*/
		socket_type _server_fd;

		Socket& operator=(const Socket& cls);
		Socket(const Socket& cls);
		Socket();

	public:
		Socket(const Configure& cls);
		~Socket();

		void request_handler();
	};
}
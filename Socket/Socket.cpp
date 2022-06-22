#include "Socket.hpp"

ws::Socket::Socket(const ws::Configure& cls) {
	std::vector<ws::Server> server = cls.get_server();

	for (int i = 0; i < server.size(); i++) {
		ws::Server::listen_type listen = server[i].get_listen();

		for (int j = 0; j < server[i].get_listen().size(); j++){
			std::pair<int, int> server_fd;
			if ((server_fd.first = socket(PF_INET, SOCK_STREAM, 0)) == -1)
				throw; // require custom exception
			if ((server_fd.second = kqueue()) == -1)
				throw; // reuqire custom exception
			_server_fd.push_back(server_fd);

			struct sockaddr_in server_addr;

			memset(&server_addr, 0, sizeof(server_addr));

			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = listen[j].first;
			server_addr.sin_port = listen[j].second;

			_server_addr.push_back(server_addr);
		}
	}

	for (int i = 0; i < _server_fd.size(); i++) {
		if (bind(_server_fd[i].first, (struct sockaddr*)&_server_addr[i], sizeof(_server_addr[i])) == -1)
			throw; // reuqire custom exception
		if (listen(_server_fd[i].first, 5) == -1)
			throw; // reuqire custom exception
		fcntl(_server_fd[i].first, F_SETFL, O_NONBLOCK);
	}
}

ws::Socket::~Socket() {}

void ws::Socket::request_handler() {
	
}
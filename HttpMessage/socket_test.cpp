#include <sstream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

// using namespace std;

#define BUFFER_SIZE 128
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

void exit_with_perror(const std::string& msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

int main(int ac, char **av)
{
	(void) ac;
	std::map<int, struct sockaddr_in> server;
	std::vector<struct kevent> change_list;


	for (int i = 0; i < 10; i++) {
		std::pair<int, struct sockaddr_in> server_pair;
		int fd;
		struct sockaddr_in	server_addr;

		if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
			std::cerr << "sock error: " << std::strerror(errno) << std::endl;
			exit(1);
		}
		server_pair.first = fd;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(std::stoi(av[1]) + i);
		server_pair.second = server_addr;
		if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
			exit_with_perror("bind() error: " + std::string(strerror(errno)));
		if (listen(fd, 5) == -1)
			exit_with_perror("listen() error: " + std::string(strerror(errno)));
		fcntl(fd, F_SETFL, O_NONBLOCK);
		// EV_SET(&change_list[i], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		change_events(change_list, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		server.insert(server_pair);
	}

	// for (std::map<int, struct sockaddr_in>::iterator it = server.begin(); it != server.end(); it++) {
	// 	std::cout << YLW << "\n=====================================\n" << NC << std::endl;
	// 	std::cout << "Server socket fd: " << it->first << std::endl;
	// 	std::cout << "Server socket family: " << it->second.sin_family << std::endl;
	// 	std::cout << "Server socket addr: " << ntohs(it->second.sin_addr.s_addr) << std::endl;
	// 	std::cout << "Server socket port: " << ntohs(it->second.sin_port) << std::endl;
	// 	std::cout << YLW << "\n=====================================\n" << NC << std::endl;
	// }

	int kq;
	if ((kq = kqueue()) == -1)
		exit_with_perror("kqueue() error: " + std::string(strerror(errno)));

	std::cout << "server started" << std::endl;

	int new_event = 0;
	int event_size;
	struct kevent event_list[80];
	struct kevent event_temp[80];
	struct kevent* curr_event;
	memset(&event_temp, 0, sizeof(event_temp));
	std::map<int, int> client;
	int port = -1;

	while (1) {

		new_event = kevent(kq, &change_list[0], change_list.size(), event_temp, 80, NULL);
		if (new_event == -1)
			exit_with_perror("kevent() error: " + std::string(strerror(errno)));
		change_list.clear();

		for (int i = 0; i < new_event; i++) {
			curr_event = &event_temp[i];
			std::map<int, struct sockaddr_in>::iterator server_iter = server.find(curr_event->ident);
			std::map<int, int>::iterator client_iter = client.find(curr_event->ident);
			int client_socket_fd;

			if (curr_event->filter == EVFILT_READ) {
				if (server_iter != server.end()) {
					port = ntohs(server_iter->second.sin_port);
					if ((client_socket_fd = accept(curr_event->ident, NULL, NULL)) == -1)
						exit_with_perror("accept() error: " + std::string(strerror(errno)));
					fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
					// change_events(change_list, client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					std::cout << "[Client connected] - server_port: " << port << ", server_fd: " << server_iter->first << ", client_fd: " << client_socket_fd << std::endl;
					change_events(change_list, client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					client.insert(std::pair<int, int>(client_socket_fd, curr_event->ident));
				}
				else if (client_iter != client.end()) {
					server_iter = server.find(client.find(client_iter->first)->second);
					std::cout << "[Client request] - server_port: " << port << ", server_fd: " << server_iter->first << ", client_fd: " << client_iter->first << std::endl;
					std::cout << YLW << "\n=== Request Data ==========================" << NC << std::endl;
					char buf[4096];
					int n = read(curr_event->ident, buf, sizeof(buf));
					std::cout << buf << std::endl;
					std::cout << YLW << "===========================================\n" << NC << std::endl;
					change_events(change_list, client_iter->first, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				}
				else {
					std::cout << "[READ Error] - server_port: " << port << ", server_fd: " << server_iter->first << ", client_fd: " << client_iter->first << std::endl;
				}
			}
			else if (curr_event->filter == EVFILT_WRITE) {
				std::string body = "server_port: " + std::to_string(port) + ", server_fd: " + std::to_string(server_iter->first) + ", client_fd: " + std::to_string(client_iter->first);
				std::string response = std::string("HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\nContent-Type: text\r\nDate: Mon, 20 Jun 2022 02:59:03 GMT\r\nETag: \"62afd0a1-267\"\r\nLast-Modified: Mon, 20 Jun 2022 01:42:57 GMT\r\nServer: webserv\r\n\r\n") + body;
				write(curr_event->ident, response.c_str(), response.size());
				client.erase(curr_event->ident);
				close(curr_event->ident);
				// change_events(change_list, curr_event->ident, EV_CLEAR, EV_)
			}
		}
	}
}
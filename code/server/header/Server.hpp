#ifndef SERVER_HPP
#define SERVER_HPP
#define PORT 6667

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <pthread.h>
#include <Client.hpp>
#include <poll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <replies.hpp>

class Server
{
	private:
		std::map<int, Client> users;
		std::string passwd;
		int socketfd;
		sockaddr_in serverAddress;

	public:
		Server(const std::string& password = "");
		~Server();
		struct sockaddr_in &getServerAddress();
		void setServerAddress(struct sockaddr_in serverAddress);
		int getSocketfd();
		void setSocketfd(int socketfd);
		void addUser(int socketfd, Client client);
};

int server();

#endif
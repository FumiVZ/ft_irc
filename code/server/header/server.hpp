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
#include <client.hpp>
#include <poll.h>
#include <cstring>
#include <errno.h>
#include <vector>

enum Command
{
	PASS,
	NICK,
	USER,
	OPER,
	MODE,
	QUIT,
	JOIN,
	PART,
	TOPIC,
	KICK,
	PRIVMSG,
	NOTICE,
	SENDFILE,
};

class Server
{
	private:
		std::map<std::string, Client> users;
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
};

int server();
void	parseCommand(char *buffer);
#endif
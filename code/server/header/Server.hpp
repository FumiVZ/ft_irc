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

#include <Message.hpp>
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
#include <cstring>
#include <errno.h>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <replies.hpp>

class Server
{
	private:
		std::map<int, Client> users;
		std::map<std::string, Channel> channels;
		std::vector<pollfd> fds;
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
		bool isClientAuthenticated(int clientSocket);
		bool isNicknameInUse(const std::string &nickname);
		void addUser(int socketfd, Client client);
		void broadcast(std::string message);
		Client & getClient(int socketfd);
		Client & getClient(const std::string &nickname);
		const std::string &getPasswd();
		void addChannel(Channel &ch);
		Channel *getChannel(const std::string &name);
		void setFds(std::vector<pollfd> fds);
		std::vector<pollfd> &getFds();
		void removeUser(int socketfd, std::vector<pollfd> &fds);
		void removeEmptyChannels();
};

int server();
void	parseCommand(Server &server, int clientSocket, Message message);
std::string get_ip(struct in_addr *in);
std::string get_hostname(struct sockaddr_in &clientAddr);
std::string get_g_hostname();
#endif
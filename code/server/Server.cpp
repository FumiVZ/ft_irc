#include <Server.hpp>
#include <cstring>
#include <errno.h>
#include <vector>
#include <sstream>

#define DEBUG 1

std::string get_ip(struct in_addr *in)
{
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, in, clientIp, INET_ADDRSTRLEN);
	return clientIp;
}

std::string get_hostname(struct sockaddr_in &clientAddr)
{
	char clientHostname[NI_MAXHOST];
	if (getnameinfo((struct sockaddr *)&clientAddr, sizeof(clientAddr), clientHostname, NI_MAXHOST, NULL, 0, 0) != 0)
	{
		return get_ip(&clientAddr.sin_addr);
	}
	return clientHostname;
}

bool Server::isNicknameInUse(const std::string &nickname)
{
	for (std::map<int, Client>::iterator i = this->users.begin(); i != this->users.end(); i++)
	{
		if (i->second.getNickname() == nickname)
			return true;
	}
	return false;
}

void Server::setSocketfd(int socketfd)
{
	this->socketfd = socketfd;
}

int Server::getSocketfd()
{
	return this->socketfd;
}

struct sockaddr_in &Server::getServerAddress()
{
	return this->serverAddress;
}

void Server::setServerAddress(struct sockaddr_in serverAddress)
{
	this->serverAddress = serverAddress;
}

Client &Server::getClient(int socketfd)
{
	return this->users.at(socketfd);
}

Client &Server::getClient(const std::string &nickname)
{
	for (std::map<int, Client>::iterator i = this->users.begin(); i != this->users.end(); i++)
	{
		if (i->second.getNickname() == nickname)
			return i->second;
	}
	throw std::runtime_error("No client with nickname " + nickname);
}

Server::Server(const std::string &password)
{
	this->passwd = password;
	this->socketfd = 0;
	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->serverAddress.sin_port = htons(PORT);
}

Server::~Server()
{
	close(this->socketfd);
}

const std::string &Server::getPasswd()
{
	return this->passwd;
}

struct sockaddr_in acceptClient(Server &server, std::vector<pollfd> &fds)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(server.getSocketfd(), (struct sockaddr *)&clientAddr, &clientLen);
	if (clientSocket < 0)
		throw std::runtime_error("Acceptation error: " + std::string(strerror(errno)));
	pollfd clientPoll;
	Client client(clientSocket, get_ip(&clientAddr.sin_addr), get_hostname(clientAddr));
	clientPoll.fd = clientSocket;
	clientPoll.events = POLLIN;
	fds.push_back(clientPoll);
	std::cout << "New connection at " << get_ip(&clientAddr.sin_addr) << " (" << get_hostname(clientAddr) << ")" << std::endl;
	server.addUser(clientSocket, client);
	if (DEBUG == 1)
	{
		std::string nickname = "test";
		server.getClient(clientSocket).setAuth(true);
		if (!(server.isNicknameInUse(nickname)))
			server.getClient(clientSocket).setNickname("test");
		else
			server.getClient(clientSocket).setNickname("test1");
		server.getClient(clientSocket).setUsername("test");
		server.getClient(clientSocket).setHostname("test");
		std::ostringstream oss;
		oss << clientSocket;
		server.getClient(clientSocket).sendReply("001", "User " + server.getClient(clientSocket).getNickname() + "nick: " + server.getClient(clientSocket).getNickname() + "clientsocket: " + oss.str());
	}
	rpl_welcome(client);
	return clientAddr;
}

void serverCreation(Server &server)
{
	server.setSocketfd(socket(AF_INET, SOCK_STREAM, 0));
	if (server.getSocketfd() < 0)
	{
		std::cerr << "Error socket: " << strerror(errno) << std::endl;
		return;
	}
	int opt = 1;
	if (setsockopt(server.getSocketfd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error setsockopt: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
	if (bind(server.getSocketfd(), (struct sockaddr *)&server.getServerAddress(), sizeof(server.getServerAddress())) < 0)
	{
		std::cerr << "Error de bind: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
	if (listen(server.getSocketfd(), 5) < 0)
	{
		std::cerr << "Error listen: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
}

bool pass(Server &server, int clientSocket, char *password)
{
	password[strlen(password) - 1] = '\0';
	std::cout << "Password received: " << password << std::endl;
	std::cout << "Password expected: " << server.getPasswd() << std::endl;
	if (strcmp(password, server.getPasswd().c_str()) == 0)
	{
		std::cout << "Authentification réussie" << std::endl;
		server.getClient(clientSocket).setAuthentified();
		send(clientSocket, "Authentification success!\r\n", 27, 0);
		return true;
	}
	server.getClient(clientSocket).sendReply("464", ERR_PASSWDMISMATCH);
	return false;
}

bool Server::isClientAuthenticated(int clientSocket)
{
	return this->users.at(clientSocket).isAuthentified();
}

// NEED FIX DONT WORK
// void receiveMessage(Server &server, int clientSocket)
// {
// 	char buffer[1024] = {0};
// 	int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
// 	std::cout << "Message received: " << buffer << std::endl;

// 	if (n < 0)
// 	{
// 		std::cerr << "Error recv: " << strerror(errno) << std::endl;
// 		close(clientSocket);
// 		return;
// 	}
// 	else if (n == 0)
// 	{
// 		std::cout << "Client disconnected" << std::endl;
// 		close(clientSocket);
// 		return;
// 	}
// 	else
// 	{
// 		buffer[n] = '\0';
// 		std::string message(buffer);
// 		message.erase(std::remove(message.begin(), message.end(), '\r'), message.end());
// 		std::vector<std::string> messages;
// 		size_t pos = 0;
// 		std::string delimiter = "\r\n";
// 		while ((pos = message.find(delimiter)) != std::string::npos)
// 		{
// 			messages.push_back(message.substr(0, pos));
// 			message.erase(0, pos + delimiter.length());
// 		}
// 		if (!message.empty())
// 		{
// 			messages.push_back(message);
// 		}
// 		for (size_t i = 0; i < messages.size(); ++i)
// 		{
// 			if (!messages[i].empty())
// 			{
// 				if ((strncmp(messages[i].c_str(), "PASS", 4) != 0) && server.isClientAuthenticated(clientSocket) == 0)
// 				{
// 					server.getClient(clientSocket).sendReply("451", "You have not registered yet use PASS <password>");
// 					return;
// 				}
// 				else if (strncmp(messages[i].c_str(), "PASS", 4) == 0 && server.isClientAuthenticated(clientSocket) == 0)
// 				{
// 					char passwordCopy[1024];
// 					strncpy(passwordCopy, messages[i].c_str() + 5, sizeof(passwordCopy) - 1);
// 					passwordCopy[sizeof(passwordCopy) - 1] = '\0';
// 					pass(server, clientSocket, passwordCopy);
// 					return;
// 				}
// 				else
// 					parseCommand(server, clientSocket, const_cast<char*>(messages[i].c_str()));
// 			}
// 		}
// 	}
// }

void receiveMessage(Server &server, int clientSocket)
{
	char buffer[1024] = {0};
	int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	std::vector<std::string> messages;
	
	std::cout << "Message received: " << buffer << std::endl;
	if (n < 0)
	{
		std::cerr << "Error recv: " << strerror(errno) << std::endl;
		close(clientSocket);
		return;
	}
	else if (n == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		close(clientSocket);
		return;
	}
	else
	{
		buffer[n] = '\0';
		if ((strncmp(buffer, "PASS", 4) != 0) && server.isClientAuthenticated(clientSocket) == 0)
		{
			server.getClient(clientSocket).sendReply("451", "You have not registered yet use PASS <password>");
			return;
		}
		else if (strncmp(buffer, "PASS", 4) == 0 && server.isClientAuthenticated(clientSocket) == 0)
		{
			pass(server, clientSocket, buffer + 5);
			return;
		}
		else
			parseCommand(server, clientSocket, buffer);
	}
}

void Server::addUser(int socketfd, Client client)
{
	this->users.insert(std::pair<int, Client>(socketfd, client));
}

int server()
{
	Server server("password");
	std::vector<pollfd> fds;
	serverCreation(server);
	pollfd serverPoll;
	serverPoll.fd = server.getSocketfd();
	serverPoll.events = POLLIN;
	fds.push_back(serverPoll);
	std::cout << "Server start on PORT: " << PORT << std::endl;
	while (true)
	{
		int poll_result = poll(&fds[0], fds.size(), -1);
		if (poll_result < 0)
		{
			std::cerr << "Poll error. " << strerror(errno) << std::endl;
			continue;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{

			if (fds[i].revents && fds[i].revents & POLLIN)
			{
				if (fds[i].fd == server.getSocketfd())
				{
					try
					{
						acceptClient(server, fds);
					}
					catch (const std::runtime_error &e)
					{
						continue;
					}
				}
				else
					receiveMessage(server, fds[i].fd);
			}
		}
	}
	return 0;
}

void Server::broadcast(std::string message)
{
	for (std::map<int, Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
	{
		send(it->first, message.c_str(), message.length(), 0);
	}
}

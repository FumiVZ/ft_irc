#include <Server.hpp>
#include <cstring>
#include <errno.h>
#include <vector>
#include <sstream>

#define DEBUG 1

static sockaddr_in g_serverAddr;

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

std::string get_g_hostname() { return get_hostname(g_serverAddr); }

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
	g_serverAddr = this->serverAddress;
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
	clientPoll.revents = 0;
	fds.push_back(clientPoll);
	std::cout << "New connection at " << get_ip(&clientAddr.sin_addr) << " (" << get_hostname(clientAddr) << ")" << std::endl;
	server.addUser(clientSocket, client);
	if (DEBUG == 1)
	{
		std::cout << "debug Client" << std::endl;
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

bool pass(Server &server, int clientSocket, const char *password)
{
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

void clear_buffer(char *buffer)
{
	for (size_t i = 0; i < sizeof(buffer); i++)
		buffer[i] = '\0';
}

std::string MakeVisible(std::string str)
{
	std::string result;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '\r')
			result += "\\r";
		else if (str[i] == '\n')
			result += "\\n";
		else
			result += str[i];
	}
	return result;
}

void receiveMessage(Server &server, int clientSocket)
{
	char buffer[1024] = {0};
	int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (n < 0)
	{
		std::cerr << "Error recv: " << strerror(errno) << std::endl;
		close(clientSocket);
		return;
	}
	else if (n == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		server.getClient(clientSocket).setAuth(false);
		close(clientSocket);
		return;
	}
	std::string message(buffer, n);
	if ((message.find("\r\n") == std::string::npos))
	{
		message = message + "\r\n";
		std::cout << "Message: " << MakeVisible(message) << std::endl;
	}
	else
	{
		std::cout << "Message: " << MakeVisible(message) << std::endl;
	}
	size_t start = 0;
	size_t end;
	while ((end = message.find("\r\n", start) + 2) != std::string::npos)
	{
		try
		{
			Message new_mess(message.substr(start, end - start));
			if (new_mess.getRawMessage().empty())
			{
				start = end;
				continue;
			}
			try
			{
				if (!server.isClientAuthenticated(clientSocket))
				{
					if (new_mess.getCommand() == "PASS")
						pass(server, clientSocket, new_mess.getParameters()[0].c_str());
					else
						server.getClient(clientSocket).sendReply("451", ERR_NOTREGISTERED);
				}
				else
					parseCommand(server, clientSocket, new_mess);
			}
			catch (const std::invalid_argument &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				start = end;
				continue;
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
		if (end == message.size())
			break;
	}
}

void auth_client(Server &server, int clientSocket, Message message)
{
	if (message.getCommand() == "PASS")
	{
		pass(server, clientSocket, message.getParameters()[0].c_str());
	}
	else
	{
		server.getClient(clientSocket).sendReply("451", ERR_NOTREGISTERED);
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
			if (fds[i].revents & POLLIN)
			{
				if (i == 0 && fds[i].fd == server.getSocketfd())
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
				{
					receiveMessage(server, fds[i].fd);
				}
			}
			if (fds[i].revents & POLLHUP)
			{
				std::cout << "Client disconnected" << std::endl;
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
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

void Server::addChannel(Channel &ch) { this->channels.insert(std::pair<std::string, Channel>(ch.getName(), ch)); }
Channel *Server::getChannel(const std::string &name)
{
	Channel *ch = NULL;
	try
	{
		ch = &(this->channels.at(name));
	}
	catch (const std::out_of_range &)
	{
		return NULL;
	}
	return ch;
}
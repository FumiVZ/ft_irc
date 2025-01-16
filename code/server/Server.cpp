#include <Server.hpp>
#include <cstring>
#include <errno.h>
#include <vector>

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

std::string get_g_hostname()
{
	return get_hostname(g_serverAddr);
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

void serverCreation(Server &server)
{
	server.setSocketfd(socket(AF_INET, SOCK_STREAM, 0));
	if (server.getSocketfd() < 0)
	{
		std::cerr << "Erreur de création de socket: " << strerror(errno) << std::endl;
		return;
	}
	int opt = 1;
	if (setsockopt(server.getSocketfd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Erreur de setsockopt: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
	if (bind(server.getSocketfd(), (struct sockaddr *)&server.getServerAddress(), sizeof(server.getServerAddress())) < 0)
	{
		std::cerr << "Erreur de bind: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
	if (listen(server.getSocketfd(), 5) < 0)
	{
		std::cerr << "Erreur de listen: " << strerror(errno) << std::endl;
		close(server.getSocketfd());
		return;
	}
}

bool pass(Server &server, int clientSocket, char *password)
{
	if (strcmp(password, server.getPasswd().c_str()) == 0)
	{
		std::cout << "Authentification réussie" << std::endl;
		server.getClient(clientSocket).setAuthentified();
		return true;
	}
	return false;
}

struct sockaddr_in acceptClient(Server &server, std::vector<pollfd> &fds)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(server.getSocketfd(), (struct sockaddr *)&clientAddr, &clientLen);
	if (clientSocket < 0)
	{
		std::cerr << "Erreur d'acceptation: " << strerror(errno) << std::endl;
		throw std::runtime_error("Erreur d'acceptation");
	}
	pollfd clientPoll;
	Client client(clientSocket, get_ip(&clientAddr.sin_addr), get_hostname(clientAddr));
	clientPoll.fd = clientSocket;
	clientPoll.events = POLLIN;
	fds.push_back(clientPoll);
	std::cout << "Nouvelle connexion acceptée de " << get_ip(&clientAddr.sin_addr) << " (" << get_hostname(clientAddr) << ")" << std::endl;
	server.addUser(clientSocket, client);
	client.setUsername("supe4cookie");
	client.setNickname("cookie");
	rpl_welcome(client);
	return clientAddr;
}

bool Server::isClientAuthenticated(int clientSocket)
{
	return this->users.at(clientSocket).isAuthentified();
}

#
void receiveMessage(Server &server, int clientSocket)
{
	char buffer[1024] = {0};
	int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (n < 0)
	{
		std::cerr << "Erreur de recv: " << strerror(errno) << std::endl;
		close(clientSocket);
		return;
	}
	else if (n == 0)
	{
		std::cout << "Client déconnecté" << std::endl;
		close(clientSocket);
	}
	else
	{
		buffer[n] = '\0';
		std::cout << "Message recu: " << server.isClientAuthenticated(clientSocket) << std::endl;
		if ((strncmp(buffer, "PASS", 4) != 0) && server.isClientAuthenticated(clientSocket) == 0)
		{
			const char *msg = "Erreur : Vous devez vous authentifier avec la commande PASS\r\n";
			send(clientSocket, msg, strlen(msg), 0);
			return;
		}
		else if (strncmp(buffer, "PASS", 4) == 0 && server.isClientAuthenticated(clientSocket) == 0)
		{
			std::cout << "PASS" << std::endl;
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
	std::cout << "Serveur démarré sur le port " << PORT << std::endl;
	while (true)
	{
		int poll_result = poll(&fds[0], fds.size(), -1);
		if (poll_result < 0)
		{
			std::cerr << "Erreur de poll: " << strerror(errno) << std::endl;
			continue;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
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

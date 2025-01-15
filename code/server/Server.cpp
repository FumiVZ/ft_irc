#include <Server.hpp>

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

bool Server::authenticateClient(int clientSocket, const char *password)
{
	if (this->passwd == password)
	{
		this->users[clientSocket] = Client(clientSocket);
		return true;
	}
	return false;
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

struct sockaddr_in acceptClient(Server &server, std::vector<pollfd> &fds)
{
	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);
	int clientSocket = accept(server.getSocketfd(), (struct sockaddr *)&clientAddress, &clientAddressSize);
	if (clientSocket < 0)
	{
		std::cerr << "Erreur de accept: " << strerror(errno) << std::endl;
		return clientAddress;
	}
	pollfd clientPoll;
	clientPoll.fd = clientSocket;
	clientPoll.events = POLLIN;
	fds.push_back(clientPoll);
	std::cout << "Client connecté" << std::endl;
	return clientAddress;
}

bool Server::isClientAuthenticated(int clientSocket)
{
	return this->users.find(clientSocket) != this->users.end();
}

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
		std::cout << "Message reçu (taille " << n << "): '" << buffer << "'" << std::endl;
		if (strncmp(buffer, "PASS", 4) != 0 && !server.isClientAuthenticated(clientSocket))
		{
			const char *msg = "Erreur : Vous devez d'abord vous authentifier avec PASS\r\n";
			send(clientSocket, msg, strlen(msg), 0);
			return;
		}
		else if (strncmp(buffer, "PASS", 4) == 0)
			
	}
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
					acceptClient(server, fds);
				else
					receiveMessage(server, fds[i].fd);
			}
		}
	}
	return 0;
}

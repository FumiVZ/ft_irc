#include <Server.hpp>
#include <cstring> // pour strerror
#include <errno.h> // pour errno
#include <vector>

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

int server()
{
	Server server("password");
	std::vector<pollfd> fds;
	pollfd serverPoll;

	server.setSocketfd(socket(AF_INET, SOCK_STREAM, 0));
	if (server.getSocketfd() < 0)
	{
		std::cerr << "Erreur lors de la création du socket" << std::endl;
		return 1;
	}

	if (bind(server.getSocketfd(), (struct sockaddr *)&server.getServerAddress(), sizeof(server.getServerAddress())) < 0)
	{
		std::cerr << "Erreur lors du binding: " << strerror(errno) << std::endl;
		return 1;
	}

	if (listen(server.getSocketfd(), SOMAXCONN) < 0)
	{
		std::cerr << "Erreur lors de l'écoute: " << strerror(errno) << std::endl;
		return 1;
	}

	serverPoll.fd = server.getSocketfd();
	serverPoll.events = POLLIN;
	fds.push_back(serverPoll);

	std::cout << "Serveur démarré sur le port " << PORT << std::endl;

	while (true)
	{
		if (poll(fds.data(), fds.size(), -1) < 0)
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
					struct sockaddr_in clientAddr;
					socklen_t clientLen = sizeof(clientAddr);
					int clientSocket = accept(server.getSocketfd(), (struct sockaddr *)&clientAddr, &clientLen);

					if (clientSocket < 0)
					{
						std::cerr << "Erreur d'acceptation: " << strerror(errno) << std::endl;
						continue;
					}

					pollfd clientPoll;
					clientPoll.fd = clientSocket;
					clientPoll.events = POLLIN;
					fds.push_back(clientPoll);

					std::cout << "Nouvelle connexion acceptée" << std::endl;
				}
				else
				{
					char buffer[1024] = {0};
					ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);

					if (bytesRead <= 0)
					{
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						i--;
						std::cout << "Client déconnecté" << std::endl;
					}
					else
					{
						std::cout << "Message reçu : " << buffer << std::endl;
					}
				}
			}
		}
	}
	return 0;
}
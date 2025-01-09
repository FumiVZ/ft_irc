#include <server.hpp>
#include <cstring> // pour strerror
#include <errno.h> // pour errno
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
	server.setSocketfd(socket(AF_INET, SOCK_STREAM, 0));
	if (server.getSocketfd() < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}

	if (bind(server.getSocketfd(), (struct sockaddr *)&server.getServerAddress(), sizeof(server.getServerAddress())) < 0)
	{
		std::cerr << "Error binding socket: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
		return 1;
	}

	if (listen(server.getSocketfd(), 5) < 0)
	{
		std::cerr << "Error listening on socket: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
		return 1;
	}

	std::cout << "Server started on port " << PORT << std::endl;

	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	while (true)
	{
		int clientSocket = accept(server.getSocketfd(), (struct sockaddr *)&clientAddr, &clientLen);
		if (clientSocket < 0)
		{
			std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
			continue;
		}
		std::cout << "Nouvelle connexion acceptée" << std::endl;
		char buffer[1024] = {0};
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead > 0)
		{
			std::cout << "Message reçu : " << buffer << std::endl;
		}

		close(clientSocket);
	}

	return 0;
}

#include <Client.hpp>

Client::Client(int socketfd)
{
	this->socketfd = socketfd;
	this->is_auth = false;
};
Client::~Client()
{
};

bool Client::isAuthentified()
{
	return this->is_auth;
}

void Client::setAuthentified()
{
	this->is_auth = true;
}

int Client::getSocketfd() const
{
	return this->socketfd;
}

int client()
{
	return 0;
}
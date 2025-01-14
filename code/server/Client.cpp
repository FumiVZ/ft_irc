#include <Server.hpp>
#include <Client.hpp>

Client::Client() {};
Client::Client(int socketfd, std::string ip, std::string hostname) : 
	nickname(""),
	username(""),
	ip(ip),
	hostname(hostname),
	full_client_id(""),
	is_auth(true),
	channels(),
	socketfd(socketfd)
{};
Client::~Client() {};

void Client::setIp(std::string ip) { this->ip = ip; }
std::string Client::getIp() { return this->ip; }

int client()
{
	return 0;
}

void Client::setSocketfd(int socketfd) { this->socketfd = socketfd; }
int Client::getSocketfd() { return this->socketfd; }
void Client::setAuth(bool auth) { this->is_auth = auth; }
bool Client::getAuth() { return this->is_auth; }
std::string Client::getNickname() { return this->nickname; }
void Client::setNickname(std::string nickname) { this->nickname = nickname; }
std::string Client::getUsername() { return this->username; }
void Client::setUsername(std::string username) { this->username = username; }
std::string Client::getHostname() { return this->hostname; }
void Client::setHostname(std::string hostname) { this->hostname = hostname; }

void Client::sendReply(std::string code, std::string message)
{
	std::string reply = ":" + this->getHostname() + " " + code + " " + message + "\r\n";
	send(this->getSocketfd(), reply.c_str(), reply.size(), 0);
}

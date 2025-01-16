#include <Server.hpp>
#include <Client.hpp>

Client::Client() {};
Client::Client(int socketfd, std::string ip, std::string hostname) : 
	nickname(""),
	username(""),
	ip(ip),
	hostname(hostname),
	full_client_id(""),
	channels(),
	socketfd(socketfd),
	is_auth(false)
{};
Client::~Client() {};

void Client::setIp(std::string ip) { this->ip = ip; }
std::string Client::getIp() { return this->ip; }

bool Client::isAuthentified()
{
	return this->is_auth;
}

void Client::setAuthentified()
{
	this->is_auth = true;
}

int client()
{
	return 0;
}

void Client::setSocketfd(int socketfd) { this->socketfd = socketfd; }
int Client::getSocketfd() const { return this->socketfd; }
void Client::setAuth(bool auth) { this->is_auth = auth; }
bool Client::getAuth() { return this->is_auth; }
std::string Client::getNickname() { return this->nickname; }
void Client::setNickname(std::string nickname) { this->nickname = nickname; }
std::string Client::getUsername() { return this->username; }
void Client::setUsername(std::string username) { this->username = username; }
std::string Client::getHostname() { return this->hostname; }
void Client::setHostname(std::string hostname) { this->hostname = hostname; }
bool Client::operator==(const Client &c) const { return this->getSocketfd() == c.getSocketfd(); }
void Client::forwardMessage(std::string message) { send(this->getSocketfd(), message.c_str(), message.size(), 0); }
void Client::sendMsg(std::string msg, Channel &ch) { ch.broadcast(*this, msg); }
void Client::sendMsg(std::string msg, Client &c) { c.forwardMessage(msg); }

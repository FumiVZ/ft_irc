#include <Server.hpp>
#include <Client.hpp>

Client::Client(int socketfd, std::string ip, std::string hostname) : 
	nickname(""),
	username(""),
	ip(ip),
	hostname(hostname),
	channels(),
	buffer(""),
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

void Client::setBuffer(std::string buffer) { this->buffer = buffer; }
std::string Client::getBuffer() { return this->buffer; }

std::vector<Channel *> &Client::getChannels() { return this->channels; }


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
void Client::sendReply(std::string code, std::string message)
{
	std::string reply = ":" + this->getHostname() + " " + code + " " + message + "\r\n";
	this->forwardMessage(reply);
}
void Client::addChannel(Channel *channel) { this->channels.push_back(channel); }

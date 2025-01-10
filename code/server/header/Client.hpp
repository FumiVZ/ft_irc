#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include <vector>
#include <string>

class Client
{
private:
	std::string nickname;
	std::string username;
	std::string ip;
	std::string hostname;
	std::string full_client_id;
	std::vector<Channel> channels;
	int socketfd;
	bool is_auth;

public:
	Client() : socketfd(-1), is_auth(false) {};
	Client(int socketfd);
	~Client();
	bool isAuthentified();
	void setAuthentified();
	int getSocketfd() const;
};

#endif
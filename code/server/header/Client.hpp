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

public:
	bool is_auth;
	Client(/* args */);
	~Client();
};



#endif
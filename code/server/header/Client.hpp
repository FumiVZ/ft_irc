#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include <vector>

class Client
{
private:
	std::string nickname;
	std::string username;
	std::string ip;
	std::string hostname;
	std::string full_client_id;
	bool is_auth;
	std::vector<Channel> channels;

public:
	Client(/* args */);
	~Client();
};



#endif
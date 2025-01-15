#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>

class Client;
class Channel
{
private:
	std::string name;
	//Client *owner;
	std::vector<Client> clients;
public:
	//Channel(std::string name, Client *owner);
	Channel(std::string name);
	void broadcast(std::string message);
	~Channel();
};


#endif
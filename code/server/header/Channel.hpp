#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>

class Client;
class Channel
{
private:
	std::string name;
	Client *owner;
	std::vector<Client> clients;

	std::string topic;
	char mode;
public:
	Channel(std::string name, Client *owner);
	void broadcast(std::string message);
	std::string getName();
	std::vector<Client> &getClients();
	~Channel();
};


#endif
#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>

class Client;
class Channel
{
private:
	std::string name;
	std::vector<Client> ops;
	std::vector<Client> clients;

	std::string topic;
	char mode;
public:
	Channel(std::string name, Client &owner);
	void broadcast(Client &c, std::string msg);
	std::string getName();
	std::vector<Client> &getClients();
	std::vector<Client> &getOps();
	void addOp(Client &op);
	bool isOp(Client &op);

	~Channel();
};


#endif
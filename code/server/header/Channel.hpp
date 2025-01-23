#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>
#include <iostream>
#include <vector>


class Client;
class Channel
{
private:
	std::string name;
	std::vector<Client> ops;
	std::vector<Client> clients;

	std::string topic;
	std::vector<char> modes;
public:
	Channel(std::string name, Client &owner);
	void broadcast(Client &c, std::string msg);
	std::string getName();
	std::string getTopic();
	std::vector<Client> &getClients();
	std::vector<Client> &getOps();
	void addOp(Client &op);
	bool isOp(Client &op);
	void addClient(Client &c);

	~Channel();
};


#endif
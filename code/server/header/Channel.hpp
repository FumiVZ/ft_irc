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
	std::string password;
	std::string topic;
	std::vector<char> modes;
	size_t limit;
public:
	Channel(std::string name, Client &owner);
	void broadcast(Client &c, std::string msg);
	std::string getName();
	std::string getTopic();
	std::vector<Client> &getClients();
	std::vector<Client> &getOps();
	void addOp(Client &op);
	void removeOp(Client &op);
	bool isOp(Client &op);
	void addClient(Client &c);
	void removeClient(Client &c);
	bool isClient(Client &c);
	void setPasswd(std::string passwd);
	void setTopic(std::string topic);
	void setLimit(size_t limit);
	size_t getLimit();
	void addMode(char mode);
	void removeMode(char mode);
	std::string getModes();
	void ChannelIsTrue(char mode);
	std::string getPasswd();
	~Channel();
};


#endif
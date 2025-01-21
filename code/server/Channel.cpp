#include <Channel.hpp>

Channel::Channel(std::string name, Client &owner) : name(name), topic("")
{
	this->ops.push_back(owner);
	this->clients.push_back(owner);
}
Channel::~Channel()
{
}

std::string Channel::getName() { return this->name; };
std::vector<Client> &Channel::getClients() { return this->clients; };
std::vector<Client> &Channel::getOps() { return this->ops; };
void Channel::addOp(Client &op) { this->ops.push_back(op); };
bool Channel::isOp(Client &op)
{
	for (std::vector<Client>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (*it == op)
			return true;
	}
	return false;
}

void Channel::broadcast(Client &c, std::string msg)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getSocketfd() != c.getSocketfd())
			it->forwardMessage(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHostname() + " PRIVMSG " + this->name + " :" + msg);
	}
}

void Channel::addClient(Client &c) { this->clients.push_back(c); }
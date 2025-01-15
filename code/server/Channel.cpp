#include <Channel.hpp>

//Channel::Channel(std::string name, Client *owner) : name(name), owner(owner)
Channel::Channel(std::string name) : name(name)//, owner(owner)
{
}

Channel::~Channel()
{
}

void Channel::broadcast(std::string message)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		it->sendReply("PRIVMSG", message);
	}
}
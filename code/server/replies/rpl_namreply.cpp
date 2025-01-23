#include <Client.hpp>
#include <Channel.hpp>
#include <Server.hpp>

void rpl_namreply(Client &c, Channel &ch)
{
	std::string reply = ":" + get_g_hostname() + " 353 " + c.getNickname() + " = " + ch.getName() + " :";
	std::vector<Client> clients = ch.getClients();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (ch.isOp(*it))
			reply += "@";
		reply += it->getNickname() + " ";
	}
	reply += "\r\n";
	c.forwardMessage(reply);
}
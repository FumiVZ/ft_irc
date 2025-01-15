#include <Client.hpp>
#include <Channel.hpp>

void rpl_namreply(Client c, Channel ch, std::string server_hostname)
{
	std::string reply = ":" + server_hostname + " 353 " + c.getNickname() + " = " + ch.getName() + " :";
	for (Client client : ch.getClients())
	{
		reply += client.getNickname() + " ";
	}
	reply += "\r\n";
	c.sendReply("353", reply);
}
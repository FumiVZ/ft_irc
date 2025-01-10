#include <Server.hpp>
#include <Client.hpp>

void rpl_welcome(Client client)
{
	client.sendReply("001", "Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname());
}
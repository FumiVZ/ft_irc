#include <Server.hpp>
#include <Client.hpp>

void rpl_welcome(Client client)
{
	client.forwardMessage(":" + get_g_hostname() 
						+ " 001 " 
						+ client.getNickname() + " :Welcome to the Internet Relay Network " 
						+ client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + "\r\n");
}
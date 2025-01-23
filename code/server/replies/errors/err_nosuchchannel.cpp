#include <Client.hpp>
#include <Server.hpp>

void err_nosuchchannel(Client c, std::string channel)
{
	std::string reply = ":" + get_g_hostname() + " 403 " + c.getNickname() + " " + channel + " :No such channel\r\n";
	c.forwardMessage(reply);
}
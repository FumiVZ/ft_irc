#include <Client.hpp>
#include <Server.hpp>

void err_nosuchnick(Client c, std::string nickname)
{
	std::string reply = ":" + get_g_hostname() + " 401 " + c.getNickname() + " " + nickname + " :No such nick/channel\r\n";
	c.forwardMessage(reply);
}
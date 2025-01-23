#include <Client.hpp>
#include <Server.hpp>

void err_erroneusnickname(Client c, std::string nickname)
{
	std::string reply = ":" + get_g_hostname() + " 432 " + c.getNickname() + " " + nickname + " :Erroneous nickname\r\n";
	c.forwardMessage(reply);
}
#include <Client.hpp>
#include <Server.hpp>

void rpl_needmoreparam(Client c, std::string command)
{
	std::string reply = ":" + get_g_hostname() + " 461 " + c.getNickname() + " " + command + " :Not enough parameters\r\n";
	c.forwardMessage(reply);
}
#include <Client.hpp>
#include <Channel.hpp>
#include <Server.hpp>

void rpl_topic(Client &c, Channel &ch)
{
	std::string reply = ":" + get_g_hostname() + " 332 " + c.getNickname() + " " + ch.getName() + " :" + ch.getTopic() + "\r\n";
	c.forwardMessage(reply);
}
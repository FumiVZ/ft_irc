#include <Client.hpp>
#include <Channel.hpp>
#include <Server.hpp>

void rpl_invite(Client &requester, Client &target, Channel *channel)
{
	std::string reply = ":" + requester.getNickname() + " INVITE " + target.getNickname() + " " + channel->getName() + "\r\n";
	target.forwardMessage(reply);
}
#include <Commands.hpp>

void invite(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 2)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " INVITE :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string nickname = message.getParameters()[0];
	std::string channel_name = message.getParameters()[1];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	Client &requester = client;
	Client &target = server.getClient(nickname);
	if (!channel->isOp(requester) && !channel->isClient(requester))
	{
		client.sendReply("442", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_NOTONCHANNEL);
		return;
	}
	if (!server.isNicknameInUse(nickname))
	{
		client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + nickname + " :" + ERR_NOSUCHNICK);
		return;
	}
	target.addChannel(channel);
	rpl_invite(requester, target, channel);
}
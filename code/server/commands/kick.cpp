#include <Commands.hpp>

void kick(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 2)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " KICK :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	upcase(channel_name);
	std::string nickname = message.getParameters()[1];
	std::string reason;
	if (message.getText().empty())
		reason = "You have been kicked :(";
	else
		reason = message.getText();
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	Client &requester = client;
	try
	{
		Client &target = server.getClient(nickname);
		if (!channel->isOp(requester) && !channel->isClient(requester))
		{
			client.sendReply("442", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_NOTONCHANNEL);
			return;
		}
		if (!channel->isClient(target))
		{
			client.sendReply("441", client.getNickname().empty() ? "* " : client.getUsername() + " " + nickname + " " + channel_name + " :" + ERR_USERNOTINCHANNEL);
			return;
		}
		if (target.getNickname().empty())
		{
			client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + nickname + " :" + ERR_NOSUCHNICK);
			return;
		}
		if (!channel->isOp(requester))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel->getName() + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(requester, " KICK " + channel->getName() + " " + target.getNickname() + " :" + reason);
		channel->removeClient(target);
		channel->removeOp(target);
		target.removeChannel(channel);
	}
	catch (const std::exception &e)
	{
		client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + nickname + " :" + ERR_NOSUCHNICK);
	}
}
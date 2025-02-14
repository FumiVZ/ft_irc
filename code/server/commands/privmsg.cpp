#include <Commands.hpp>

void privmsg(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() < 1)
	{
		client.sendReply("411", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NORECIPIENT + " (PRIVMSG)");
		return;
	}
	if (message.getText().empty())
	{
		client.sendReply("412", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NOTEXTTOSEND);
		return;
	}
	std::string target = message.getParameters()[0];
	std::string text = message.getText();
	if (target.empty())
	{
		client.sendReply("411", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NORECIPIENT + " (PRIVMSG)");
		return;
	}
	if (text.empty())
	{
		client.sendReply("412", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NOTEXTTOSEND);
		return;
	}
	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (channel == NULL)
		{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + target + " :" + ERR_NOSUCHCHANNEL);
			return;
		}
		Client &requester = client;
		if (!channel->isClient(requester))
		{
			client.sendReply("442", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel->getName() + " :" + ERR_NOTONCHANNEL);
			return;
		}
		channel->broadcastMsg(requester, " PRIVMSG " + channel->getName() + " :" + text);
	}
	else
	{
		Client &targetClient = server.getClient(target);
		if (targetClient.getNickname().empty())
		{
			client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + target + " :" + ERR_NOSUCHNICK);
			return;
		}
		targetClient.forwardMessage(":" + client.getNickname() + " PRIVMSG " + targetClient.getNickname() + " :" + text + "\r\n");
	}
}
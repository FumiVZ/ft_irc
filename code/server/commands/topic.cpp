#include <Commands.hpp>

void topic(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() < 1)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " TOPIC :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	Channel *channel = server.getChannel(message.getParameters()[0]);
	if (channel == NULL)
	{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (message.getParameters().size() == 1 && message.getText().empty())
	{
		rpl_topic(client, *channel);
	}
	else if (!message.getText().empty())
	{
		if (channel->isMode('t') && !channel->isOp(client))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel->getName() + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->setTopic(message.getText());
		channel->broadcast(client, " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else if (message.getParameters().size() == 2)
	{
		if (channel->isMode('t') && !channel->isOp(client))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel->getName() + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->setTopic(message.getParameters()[1]);
		channel->broadcast(client, " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " TOPIC :" + ERR_WRONGPARAMCOUNT);
}
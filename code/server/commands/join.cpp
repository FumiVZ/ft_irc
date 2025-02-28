#include <Commands.hpp>

bool is_valid_channel_name(const std::string &name)
{
	if (name.empty() || name.size() > MAX_CHANNEL_NAME_SIZE)
		return false;
	if (name[0] != '#')
		return false;
	if (name.find_first_of(" \a\007,:") != std::string::npos)
		return false;
	if (name.size() < 2)
		return false;
	return true;
}

void join(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() < 1)
	{
		client.sendReply("461", (client.getNickname().empty() ? "* " : client.getUsername()) + " JOIN :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	if (!is_valid_channel_name(message.getParameters()[0]))
	{
		client.sendReply("403", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	upcase(channel_name);
	Channel *channel = server.getChannel(channel_name);
	if (channel != NULL)
	{
		if (channel->isClient(client))
		{
			return;
		}
		if ((message.getParameters().size() == 1 && channel->getPasswd() != "") || (message.getParameters().size() == 2 && (channel->getPasswd() != message.getParameters()[1])))
		{
			client.sendReply("475", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_BADCHANNELKEY);
			return;
		}
		if (channel->getLimit() != 0 && channel->getClients().size() >= channel->getLimit())
		{
			client.sendReply("471", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_CHANNELISFULL);
			return;
		}
		if (channel->isMode('i') && std::find(client.getChannels().begin(), client.getChannels().end(), channel) == client.getChannels().end())
		{
			client.sendReply("473", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + channel_name + " :" + ERR_INVITEONLYCHAN);
			return;
		}
		if (!channel->isClient(client))
			channel->addClient(client);
		if (!channel->isClient(client))
			return;
		client.addChannel(channel);
		client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN " + channel->getName() + "\r\n");
		channel->broadcast(client, " JOIN " + channel->getName());
		rpl_topic(client, *channel);
		rpl_namreply(client, *channel);
		rpl_endofnames(client);
		return;
	}
	Channel *new_channel = new Channel(channel_name, client);
	server.addChannel(new_channel);
	client.addChannel(new_channel);
	client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN " + new_channel->getName() + "\r\n");
	new_channel->broadcast(client, " JOIN " + new_channel->getName());
	rpl_topic(client, *new_channel);
	rpl_namreply(client, *new_channel);
	rpl_endofnames(client);
}
#include <Server.hpp>

bool is_valid_nickname(const std::string &nickname)
{
	char valid_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-";

	if (nickname.size() > 9)
		return false;
	if (!isalpha(nickname[0]) && !strchr(SPECIAL_CHARACTERS, nickname[0]))
		return false;
	for (size_t i = 1; i < nickname.size(); i++)
	{
		if (!strchr(valid_chars, nickname[i]))
			return false;
	}
	return true;
}

void nick(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() != 1)
	{
		server.getClient(clientSocket).sendReply("431", ERR_NONICKNAMEGIVEN);
		return;
	}
	std::string nickname = message.getParameters()[0];
	if (nickname.empty())
	{
		server.getClient(clientSocket).sendReply("431", ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!is_valid_nickname(nickname))
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	if (server.isNicknameInUse(nickname))
	{
		server.getClient(clientSocket).sendReply("433", ERR_NICKNAMEINUSE);
		return;
	}
	if (!server.getClient(clientSocket).getNickname().empty())
	{
		server.broadcast(":" + server.getClient(clientSocket).getNickname() + " NICK " + nickname + "\r\n");
	}
	server.getClient(clientSocket).setNickname(nickname);
	if (server.getClient(clientSocket).isNamed())
		rpl_welcome(server.getClient(clientSocket));
}

bool is_valid_username(const std::string &username)
{
	if (username.empty() || username.size() > MAX_MESSAGE_SIZE)
		return false;
	if (username.find_first_of("\0\r\n ") != std::string::npos)
		return false;
	return true;
}

void user(Server &server, int clientSocket, Message message)
{
	if (!server.getClient(clientSocket).getUsername().empty())
	{
		server.getClient(clientSocket).sendReply("462", ERR_ALREADYREGISTERED);
		return;
	}
	std::vector<std::string> parameters = message.getParameters();
	if (parameters.size() != 3)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string username = parameters[0];
	if (username.empty())
	{
		server.getClient(clientSocket).sendReply("461", ERR_UNKNOWNCOMMAND);
		return;
	}
	if (!is_valid_username(username))
	{
		server.getClient(clientSocket).sendReply("461", ERR_USERINVALID);
		return;
	}
	server.getClient(clientSocket).setUsername(username);
	if (server.getClient(clientSocket).isNamed())
		rpl_welcome(server.getClient(clientSocket));
	if (server.getClient(clientSocket).getHexChat() == false)
		rpl_motd(server.getClient(clientSocket));
}

bool isOperator(Server &server, int clientSocket, const std::string &channel_name)
{
	return server.getChannel(channel_name)->isOp(server.getClient(clientSocket));
}

bool isNumeric(const std::string &str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	if (str.length() > 3)
		return false;
	return true;
}

/*
 MODE <channel>  <mode> <+/-> <~nickname>
— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
rateurs de canaux
— k : Définir/supprimer la clé du canal (mot de passe)
— o : Donner/retirer le privilège de l’opérateur de canal
— l : Définir/supprimer la limite d’utilisateurs pour le canal
*/
void mode(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() == 2)
	{
		Channel *channel = server.getChannel(message.getParameters()[0]);
		if (channel == NULL)
		{
			server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
			return;
		}
		server.getClient(clientSocket).sendReply("324", "RPL_CHANNELMODEIS " + channel->getName() + " +" + channel->getModes());
		return;
	}
	if (message.getParameters().size() < 3)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	std::string mode_op = message.getParameters()[1];
	std::string argument = message.getParameters()[2];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
		return;
	}
	if (mode_op != "+i" && mode_op != "-i" && mode_op != "+t" && mode_op != "-t" && mode_op != "+k" && mode_op != "-k" && mode_op != "+o" && mode_op != "-o" && mode_op != "+l" && mode_op != "-l")
	{
		server.getClient(clientSocket).sendReply("472", ERR_UNKNOWNMODE);
		return;
	}
	if (mode_op == "+t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Set the mode +t for the channel" + channel->getName());
		channel->addMode('t');
	}
	else if (mode_op == "-t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Remove the mode +t for the channel" + channel->getName());
		channel->removeMode('t');
	}
	if (message.getParameters().size() < 3)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	else if (mode_op == "+k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Set the password for the channel" + channel->getName() + " :" + argument);
		channel->setPasswd(argument);
	}
	else if (mode_op == "-k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Remove the password for the channel" + channel->getName());
		channel->setPasswd("");
	}
	else if (mode_op == "+o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		Client &op = server.getClient(argument);
		if (op.getNickname().empty())
		{
			server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Give the operator privilege to " + op.getNickname());
		channel->addOp(op);
	}
	else if (mode_op == "-o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		Client &op = server.getClient(argument);
		if (op.getNickname().empty())
		{
			server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Remove the operator privilege to " + op.getNickname());
		channel->removeOp(op);
	}
	else if (mode_op == "+l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		if (!isNumeric(argument))
		{
			server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
			return;
		}
		int limit;
		std::istringstream(argument) >> limit;
		if (limit <= 0 )
		{
			server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Set the limit of users 	for the channel" + channel->getName() + " :" + argument);
		channel->setLimit(limit);
	}
	else if (mode_op == "-l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Remove the limit of users for the channel" + channel->getName());
		channel->setLimit(0);
	}
	else if (mode_op == "-i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(server.getClient(clientSocket), "Remove the invite only mode for the channel" + channel->getName());
		channel->removeMode('i');
	}
	else if (mode_op == "+i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->addMode('i');
		channel->broadcast(server.getClient(clientSocket), "Set the invite only mode for the channel" + channel->getName());
	}
}

void quit(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() != 0)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	server.getClient(clientSocket).setAuth(false);
	throw server.getClient(clientSocket).disconnected;
}

bool is_valid_channel_name(const std::string &name)
{
	if (name.empty() || name.size() > MAX_CHANNEL_NAME_SIZE)
		return false;
	if (name[0] != '#')
		return false;
	if (name.find_first_of(" \a\007,:") != std::string::npos)
		return false;
	return true;
}

void join(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() < 1)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	if (!is_valid_channel_name(message.getParameters()[0]))
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
		return;
	}
	if (message.getParameters()[0][0]  != '#')
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
		return;
	}
	Channel *channel = server.getChannel(message.getParameters()[0]);
	Client &client = server.getClient(clientSocket);
	if (channel != NULL)
	{
		if (channel->isClient(client))
			return;
		if (channel->getPasswd() != "" && (message.getParameters().size() != 2 && channel->getPasswd() != message.getParameters()[1]))
		{
			server.getClient(clientSocket).sendReply("475", ERR_BADCHANNELKEY);
			return;
		}
		if (channel->getLimit() != 0 && channel->getClients().size() >= channel->getLimit())
		{
			server.getClient(clientSocket).sendReply("471", ERR_CHANNELISFULL);
			return;
		}
		if (channel->isMode('i') && std::find(client.getChannels().begin(), client.getChannels().end(), channel) != client.getChannels().end())
		{
			server.getClient(clientSocket).sendReply("473", ERR_INVITEONLYCHAN);
			return;
		}
		if (!channel->isClient(client))
			channel->addClient(client);
		if (!channel->isClient(client))
			return;	
		client.addChannel(channel);
		client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() 
								+ " JOIN " + channel->getName() + "\r\n");
		channel->broadcast(client, " JOIN " + channel->getName());
		rpl_topic(client, *channel);
		rpl_namreply(client, *channel);
		rpl_endofnames(client);
		return;
	}
	Channel *new_channel = new Channel(message.getParameters()[0], client);
	server.addChannel(*new_channel);
	client.addChannel(new_channel);
	client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() 
								+ " JOIN " + new_channel->getName() + "\r\n");
	new_channel->broadcast(client, " JOIN " + new_channel->getName());
	rpl_topic(client, *new_channel);
	rpl_namreply(client, *new_channel);
	rpl_endofnames(client);
}

void topic(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() < 1)
	{
		client.sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	Channel *channel = server.getChannel(message.getParameters()[0]);
	if (channel == NULL)
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
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
			client.sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->setTopic(message.getText());
		channel->broadcast(client, " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else if (message.getParameters().size() == 2)
	{
		if (channel->isMode('t') && !channel->isOp(client))
		{
			client.sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->setTopic(message.getParameters()[1]);
		channel->broadcast(client, " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else
		client.sendReply("461", ERR_WRONGPARAMCOUNT);
}

void kick(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() != 2)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	std::string nickname = message.getParameters()[1];
	std::string reason;
	if (message.getParameters().size() == 2)
		reason = "You have been kicked :(";
	else
		reason = message.getParameters()[2];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
		return;
	}
	Client &requester = server.getClient(clientSocket);
	Client &target = server.getClient(nickname);
	if (!channel->isOp(requester) && !channel->isClient(requester))
	{
		server.getClient(clientSocket).sendReply("442", ERR_NOTONCHANNEL);
		return;
	}
	if (!channel->isClient(target))
	{
		server.getClient(clientSocket).sendReply("441", ERR_USERNOTINCHANNEL);
		return;
	}
	if (target.getNickname().empty())
	{
		server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
		return;
	}
	if (!channel->isOp(requester))
	{
		server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
		return;
	}
	channel->broadcast(requester, " KICK " + channel->getName() + " " + target.getNickname() + " :" + reason);
	channel->removeClient(target);
	target.removeChannel(channel);
}

void invite(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() != 2)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string nickname = message.getParameters()[0];
	std::string channel_name = message.getParameters()[1];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
		return;
	}
	Client &requester = server.getClient(clientSocket);
	Client &target = server.getClient(nickname);
	if (!channel->isOp(requester) && !channel->isClient(requester))
	{
		server.getClient(clientSocket).sendReply("442", ERR_NOTONCHANNEL);
		return;
	}
	if (!server.isNicknameInUse(nickname))
	{
		server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
		return;
	}
	target.addChannel(channel);
	rpl_invite(requester, target, channel);
}

void privmsg(Server &server, int clientSocket, Message message)
{
	std::string target = message.getParameters()[0];
	std::string text = message.getText();
	if (target.empty())
	{
		server.getClient(clientSocket).sendReply("411", ERR_NORECIPIENT);
		return;
	}
	if (text.empty())
	{
		server.getClient(clientSocket).sendReply("412", ERR_NOTEXTTOSEND);
		return;
	}
	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (channel == NULL)
		{
			server.getClient(clientSocket).sendReply("403", ERR_NOSUCHCHANNEL);
			return;
		}
		Client &requester = server.getClient(clientSocket);
		if (!channel->isClient(requester))
		{
			server.getClient(clientSocket).sendReply("442", ERR_NOTONCHANNEL);
			return;
		}
		channel->broadcast(requester, " PRIVMSG " + channel->getName() + " :" + text);
	}
	else
	{
		Client &targetClient = server.getClient(target);
		if (targetClient.getNickname().empty())
		{
			server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
			return;
		}
		targetClient.sendReply("PRIVMSG", text);
	}
}

void parseCommand(Server &server, int clientSocket, Message message)
{
	size_t i = 0;
	const char *commands[] = {
		"NICK", // done
		"USER", // done
		"MODE", // done need to tests
		"QUIT", // done
		"JOIN", // done need to tests
		"TOPIC",   // done need to tests
		"KICK",	   // done
		"PRIVMSG", // done
		"INVITE" // done need to tests
	};
	void (*functions[])(Server &, int, Message) = {
		&nick,
		&user,
		&mode,
		&quit,
		&join,
		&topic,
		&kick,
		&privmsg,
		&invite
	};
	size_t num_commands = sizeof(commands) / sizeof(commands[0]);
	if (server.getClient(clientSocket).getNickname().empty() || server.getClient(clientSocket).getUsername().empty())
		num_commands = 2;
	while (i < num_commands)
	{
		try
		{
			if (message.getCommand() == commands[i])
			{
				functions[i](server, clientSocket, message);
				return;
			}
		}
		catch (const std::exception &e)
		{
			server.removeUser(clientSocket, server.getFds());
			break;
		}
		i++;
	}
}

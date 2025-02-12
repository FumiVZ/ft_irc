#include <Server.hpp>

bool is_valid_nickname(const std::string &nickname)
{
	char valid_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-";

	if (nickname.size() > 9)
	{
		return false;
	}
	if (!isalpha(nickname[0]) && !strchr(SPECIAL_CHARACTERS, nickname[0]))
		return false;
	if (nickname[0] == ':' || nickname[0] == '#')
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
	std::cout << "SIZE " << message.getParameters().size() << std::endl;
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 1)
	{
		client.sendReply("431", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NONICKNAMEGIVEN);
		return;
	}
	std::string nickname = message.getParameters()[0];
	if (nickname.empty())
	{
		client.sendReply("431", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!is_valid_nickname(nickname))
	{
		client.sendReply("432", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + nickname + " :" + ERR_ERRONEUSNICKNAME);
		return;
	}
	if (server.isNicknameInUse(nickname))
	{
		client.sendReply("433", (client.getNickname().empty() ? "* " : client.getUsername()) + " " + nickname + " :" + ERR_NICKNAMEINUSE);
		return;
	}
	if (!client.getNickname().empty())
		server.broadcast(":" + client.getNickname() + " NICK " + nickname + "\r\n");
	client.setNickname(nickname);
	if (!client.getUsername().empty())
	{
		rpl_welcome(client);
		rpl_motd(clientSocket);
	}
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
	Client &client = server.getClient(clientSocket);
	if (!client.getUsername().empty())
	{
		client.sendReply("462", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_ALREADYREGISTERED);
		return;
	}
	if (message.getParameters().size() != 3)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " USER :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::vector<std::string> parameters = message.getParameters();
	std::string username = parameters[0];
	if (username.empty())
	{
		client.sendReply("421", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_UNKNOWNCOMMAND);
		return;
	}
	if (!is_valid_username(username))
	{
		client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + username + " :" + ERR_USERINVALID);
		return;
	}
	client.setUsername(username);
	if (!client.getNickname().empty())
	{
		rpl_welcome(client);
		rpl_motd(clientSocket);
	}
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
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() == 1)
	{
		Channel *channel = server.getChannel(message.getParameters()[0]);
		if (channel == NULL)
		{
			client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
			return;
		}
		client.sendReply("324", "RPL_CHANNELMODEIS " + channel->getName() + " +" + channel->getModes());
		return;
	}
	if (message.getParameters().size() < 2 || message.getParameters().size() > 3)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " MODE :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	std::string mode_op = message.getParameters()[1];
	Channel *channel = server.getChannel(channel_name);
	if (channel == NULL)
	{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (mode_op != "+i" && mode_op != "-i" && mode_op != "+t" && mode_op != "-t" && mode_op != "+k" && mode_op != "-k" && mode_op != "+o" && mode_op != "-o" && mode_op != "+l" && mode_op != "-l")
	{
		client.sendReply("472", client.getNickname().empty() ? "* " : client.getUsername() + " " + mode_op + " :" + ERR_UNKNOWNMODE);
		return;
	}
	if (mode_op == "+t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Set the mode +t for the channel" + channel->getName());
		channel->addMode('t');
	}
	else if (mode_op == "-t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Remove the mode +t for the channel" + channel->getName());
		channel->removeMode('t');
	}
	else if (mode_op == "-i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Remove the invite only mode for the channel" + channel->getName());
		channel->removeMode('i');
	}
	else if (mode_op == "+i")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->addMode('i');
		channel->broadcast(client, "Set the invite only mode for the channel" + channel->getName());
	}
	else if (mode_op == "-k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Remove the password for the channel" + channel->getName());
		channel->setPasswd("");
	}
	else if (mode_op == "-l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Remove the limit of users for the channel" + channel->getName());
		channel->setLimit(0);
	}
	if (message.getParameters().size() < 3)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " MODE :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string argument = message.getParameters()[2];
	if (mode_op == "+k")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		channel->broadcast(client, "Set the password for the channel" + channel->getName() + " :" + argument);
		channel->setPasswd(argument);
	}
	else if (mode_op == "+o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		Client &op = server.getClient(argument);
		if (op.getNickname().empty())
		{
			client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + argument + " :" + ERR_NOSUCHNICK);
			return;
		}
		channel->broadcast(client, "Give the operator privilege to " + op.getNickname());
		channel->addOp(op);
	}
	else if (mode_op == "-o")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		Client &op = server.getClient(argument);
		if (op.getNickname().empty())
		{
			client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + argument + " :" + ERR_NOSUCHNICK);
			return;
		}
		channel->broadcast(client, "Remove the operator privilege to " + op.getNickname());
		channel->removeOp(op);
	}
	else if (mode_op == "+l")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			client.sendReply("482", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
		if (!isNumeric(argument))
		{
			client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " MODE :" + ERR_WRONGPARAMCOUNT);
			return;
		}
		int limit;
		std::istringstream(argument) >> limit;
		if (limit <= 0)
		{
			client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " MODE :" + ERR_WRONGPARAMCOUNT);
			return;
		}
		channel->broadcast(client, "Set the limit of users 	for the channel" + channel->getName() + " :" + argument);
		std::cout << "limit: " << limit << std::endl;
		channel->setLimit(limit);
	}
}

void quit(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 0)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " QUIT :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	client.setAuth(false);
	throw client.disconnected;
}

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
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " JOIN :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	if (!is_valid_channel_name(message.getParameters()[0]))
	{
		client.sendReply("403", client.getNickname().empty() ? "* " : client.getUsername() + " " + message.getParameters()[0] + " :" + ERR_NOSUCHCHANNEL);
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
			client.sendReply("475", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_BADCHANNELKEY);
			return;
		}
		if (channel->getLimit() != 0 && channel->getClients().size() >= channel->getLimit())
		{
			client.sendReply("471", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_CHANNELISFULL);
			return;
		}
		if (channel->isMode('i') && std::find(client.getChannels().begin(), client.getChannels().end(), channel) == client.getChannels().end())
		{
			client.sendReply("473", client.getNickname().empty() ? "* " : client.getUsername() + " " + channel_name + " :" + ERR_INVITEONLYCHAN);
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
	Channel *new_channel = new Channel(message.getParameters()[0], client);
	server.addChannel(*new_channel);
	client.addChannel(new_channel);
	client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN " + new_channel->getName() + "\r\n");
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

void kick(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 2)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " KICK :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
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
		channel->broadcast(requester, " PRIVMSG " + channel->getName() + " :" + text);
	}
	else
	{
		Client &targetClient = server.getClient(target);
		if (targetClient.getNickname().empty())
		{
			client.sendReply("401", client.getNickname().empty() ? "* " : client.getUsername() + " " + target + " :" + ERR_NOSUCHNICK);
			return;
		}
		targetClient.sendReply("PRIVMSG", text);
	}
}

void upcase(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void pass_auth(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	(void)message;
	std::cout << "pass auth" << std::endl;
	client.sendReply("462", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_ALREADYREGISTERED);
}

void parseCommand(Server &server, int clientSocket, Message message)
{
	size_t i = 0;
	Client &client = server.getClient(clientSocket);
	const char *commands[] = {
		"NICK",	   // done
		"USER",	   // done
		"PASS",	   // done
		"MODE",	   // done need to tests
		"QUIT",	   // done
		"JOIN",	   // done need to tests
		"TOPIC",   // done need to tests
		"KICK",	   // done
		"PRIVMSG", // done
		"INVITE",  // done need to tests
	};
	void (*functions[])(Server &, int, Message) = {
		&nick,
		&user,
		&pass_auth,
		&mode,
		&quit,
		&join,
		&topic,
		&kick,
		&privmsg,
		&invite,
	};
	size_t num_commands = sizeof(commands) / sizeof(commands[0]);
	if (client.getNickname().empty() || client.getUsername().empty())
		num_commands = 3;
	std::string command = message.getCommand();
	upcase(command);
	while (i < num_commands)
	{
		try
		{
			if (command == commands[i])
			{
				functions[i](server, clientSocket, message);
				return;
			}
		}
		catch (const std::exception &e)
		{
			server.removeUser(clientSocket, server.getFds());
			return;
		}
		i++;
	}
	if (client.getNickname().empty() || client.getUsername().empty())
	{
		client.sendReply("451", client.getNickname().empty() ? "* " : client.getUsername() + " :" + ERR_NOTREGISTERED);
		return;
	}
	client.sendReply("421", client.getNickname() + " " + message.getCommand() + " :" + ERR_UNKNOWNCOMMAND);
}

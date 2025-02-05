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
	if (!server.getClient(clientSocket).getNickname().empty())
		rpl_welcome(server.getClient(clientSocket));
}

void oper(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
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
	return true;
}

// MODE <channel>  <mode> <+/-> <~nickname>
/*— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
rateurs de canaux
— k : Définir/supprimer la clé du canal (mot de passe)
— o : Donner/retirer le privilège de l’opérateur de canal
— l : Définir/supprimer la limite d’utilisateurs pour le canal*/
void mode(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() < 3)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	std::string mode = message.getParameters()[1];
	std::string mode_op = message.getParameters()[2];
	if (mode_op != "+" && mode_op != "-")
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	if (mode == "o" || mode == "k" || mode == "l" || mode == "t")
	{
		if (!isOperator(server, clientSocket, channel_name))
		{
			server.getClient(clientSocket).sendReply("482", ERR_CHANOPRIVSNEEDED);
			return;
		}
	}
	if (mode_op == "+")
	{
		if (mode == "o")
		{
			server.getChannel(channel_name)->addOp(server.getClient(clientSocket));
		}
		else if (mode == "k")
		{
			if (message.getParameters().size() < 4)
			{
				server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
				return;
			}
			server.getChannel(channel_name)->setPasswd(message.getParameters()[3]);
		}
		else if (mode == "l")
		{
			if (message.getParameters().size() < 4 || !isNumeric(message.getParameters()[3])) // Vérification de l'argument de limite
			{
				server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
				return;
			}
			server.getChannel(channel_name)->setLimit(std::atoi(message.getParameters()[3].c_str()));
		}
		else if (mode == "t")
			server.getChannel(channel_name)->setTopicIsTrue(true);
		else
		{
			server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
			return;
		}
	}
	else if (mode_op == "-")
	{
		if (mode == "o")
			server.getChannel(channel_name)->removeOp(server.getClient(clientSocket));
		else if (mode == "k")
			server.getChannel(channel_name)->setPasswd("");
		else if (mode == "l")
			server.getChannel(channel_name)->setLimit(0);
		else if (mode == "t")
			server.getChannel(channel_name)->setTopicIsTrue(false);
	}
	else
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
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
		err_nosuchchannel(server.getClient(clientSocket), message.getParameters()[0]);
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
		channel->addClient(client);
		if (!channel->isClient(client))
			return;
		client.addChannel(channel);
		channel->broadcast(client, " JOIN " + channel->getName());
		client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() 
								+ " JOIN " + channel->getName() + "\r\n");
		rpl_topic(client, *channel);
		rpl_namreply(client, *channel);
		rpl_endofnames(client);
		return;
	}
	Channel *new_channel = new Channel(message.getParameters()[0], client);
	server.addChannel(*new_channel);
	client.addChannel(new_channel);
	new_channel->broadcast(client, " JOIN " + new_channel->getName());
	client.forwardMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() 
								+ " JOIN " + new_channel->getName() + "\r\n");
	rpl_topic(client, *new_channel);
	rpl_namreply(client, *new_channel);
	rpl_endofnames(client);
}

void part(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
}

void topic(Server &server, int clientSocket, Message message)
{
	// protect +t
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() < 1)
	{
		client.sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	Channel *channel = server.getChannel(message.getParameters()[0]);
	if (channel == NULL)
	{
		err_nosuchchannel(client, message.getParameters()[0]);
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
}

void privmsg(Server &server, int clientSocket, Message message)
{
	std::cout << "PRIVMSG" << std::endl;
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
		std::cout << "Private message" << std::endl;
		Client &targetClient = server.getClient(target);
		if (targetClient.getNickname().empty())
		{
			server.getClient(clientSocket).sendReply("401", ERR_NOSUCHNICK);
			return;
		}
		std::cout << "Sending message to: " << targetClient.getNickname() << std::endl;
		std::cout << "Message: " << text << std::endl;
		targetClient.sendReply("PRIVMSG", text);
	}
}

void notice(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
	std::cout << "clientSocket: " << clientSocket << std::endl;
	std::cout << "NICK: " << server.getClient(clientSocket).getNickname() << std::endl;
	std::cout << "USER: " << server.getClient(clientSocket).getUsername() << std::endl;
}

void sendfile(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
}

void parseCommand(Server &server, int clientSocket, Message message)
{
	size_t i = 0;
	const char *commands[] = {
		"NICK", // done
		"USER", // done
		"OPER",
		"MODE", // done need to tests
		"QUIT", // done
		"JOIN", // done need to tests
		"PART",
		"TOPIC",   // done need to tests
		"KICK",	   // done
		"PRIVMSG", // done
		"NOTICE",
		"SENDFILE",
	};
	void (*functions[])(Server &, int, Message) = {
		&nick,
		&user,
		&oper,
		&mode,
		&quit,
		&join,
		&part,
		&topic,
		&kick,
		&privmsg,
		&notice,
		&sendfile,
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
			std::cout << "Error: " << e.what() << std::endl;
			break;
		}
		i++;
	}
}

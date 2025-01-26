#include <Server.hpp>
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
	if (server.isNicknameInUse(nickname))
	{
		server.getClient(clientSocket).sendReply("433", ERR_NICKNAMEINUSE);
		return;
	}
	server.getClient(clientSocket).setNickname(nickname);
	std::cout << "Client nick is: " << server.getClient(clientSocket).getNickname() << std::endl;
}

void user(Server &server, int clientSocket, Message message)
{
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
	server.getClient(clientSocket).setUsername(username);
	std::cout << "Client username is: " << server.getClient(clientSocket).getUsername() << std::endl;
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
	close(clientSocket);
	std::cout << "Client disconnected" << std::endl;
}

void join(Server &server, int clientSocket, Message message)
{
	if (message.getParameters().size() < 1)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
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
		rpl_topic(client, *channel);
		rpl_namreply(client, *channel);
		rpl_endofnames(client);
		return;
	}
	Channel *new_channel = new Channel(message.getParameters()[0], client);
	server.addChannel(*new_channel);
	client.addChannel(new_channel);
	new_channel->broadcast(client, " JOIN " + new_channel->getName());
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
	if (message.getParameters().size() < 1)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	std::string channel_name = message.getParameters()[0];
	Channel *channel = server.getChannel(channel_name);
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
	if (message.getParameters().size() == 1)
	{
		if (channel->getTopicIsTrue())
			rpl_topic(requester, *channel);
		else
			requester.sendReply("331", "No topic is set");
		return;
	}
	std::string new_topic = message.getParameters()[1];
	channel->setTopic(new_topic);
	channel->broadcast(requester, " TOPIC " + channel->getName() + " :" + new_topic);
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
		"TOPIC", // done need to tests
		"KICK", // done
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
	if (server.getClient(clientSocket).getNickname().empty() && message.getCommand() != "NICK")
	{
		server.getClient(clientSocket).sendReply("451", ERR_NICKNAMEUNSET);
		return;
	}
	if (server.getClient(clientSocket).getUsername().empty() && message.getCommand() != "USER" && message.getCommand() != "NICK")
	{
		server.getClient(clientSocket).sendReply("451", ERR_USERNAMEUNSET);
		return;
	}

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

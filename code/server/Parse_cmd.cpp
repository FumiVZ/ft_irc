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

void mode(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
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
		channel->addClient(client);
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
	(void)server;
	(void)clientSocket;
	(void)message;
}

void kick(Server &server, int clientSocket, Message message)
{
	(void)server;
	(void)clientSocket;
	(void)message;
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
	if (strchr(target.c_str(), '#') != NULL)
	{
		std::cout << "Channel message" << std::endl;
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
		"NICK",
		"USER",
		"OPER",
		"MODE",
		"QUIT",
		"JOIN",
		"PART",
		"TOPIC",
		"KICK",
		"PRIVMSG",
		"NOTICE",
		"SENDFILE",
	};
	void (*functions[])(Server &, int, Message ) = {
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

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

}

void user(Server &server, int clientSocket, Message message)
{
	std::vector<std::string> parameters = message.getParameters();
	if (parameters.size() != 4)
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
	(void)server;
	(void)clientSocket;
	(void)message;
}

void join(Server &server, int clientSocket, Message message)
{
	Channel *channel = server.getChannel(message.getParameters()[0]);
	Client &client = server.getClient(clientSocket);
	if (channel != NULL)
	{
		channel->addClient(client);
		client.setChannel(channel);
		std::cout << "Client " << client.getNickname() << " joined channel " << channel->getName() << std::endl;
		return;
	}
	Channel newChannel(message.getParameters()[0], client);
	server.addChannel(newChannel);
	client.setChannel(&newChannel);
	std::cout << "Client " << client.getNickname() << " created channel " << newChannel.getName() << std::endl;
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
		targetClient.sendReply("PRIVMSG", server.getClient(clientSocket).getNickname() + " " + text);
		server.getClient(clientSocket).sendReply("PRIVMSG", server.getClient(clientSocket).getNickname() + " " + text);
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

void parseCommand(Server &server, int clientSocket, char *buffer)
{
	try
	{
		Message message(buffer);
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
	if (server.getClient(clientSocket).getNickname().empty() && strncmp(buffer, "NICK", 4) != 0)
	{
		server.getClient(clientSocket).sendReply("451", ERR_NICKNAMEUNSET);
		return;
	}
	if (server.getClient(clientSocket).getUsername().empty() && strncmp(buffer, "USER", 4) != 0 && strncmp(buffer, "NICK", 4) != 0)
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
				std::cout << "here" << std::endl;
				functions[i](server, clientSocket, message);
				return;
			}
		}
		catch (const std::exception &e)
		{
			break;
		}
		i++;
	}
	}
	catch (const std::invalid_argument &e)
	{
		server.getClient(clientSocket).sendReply("421", ERR_UNKNOWNCOMMAND);
	}
}

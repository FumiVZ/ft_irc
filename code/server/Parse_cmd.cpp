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
	server.getClient(clientSocket).setNickname(nickname);

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
	if (!is_valid_username(username))
	{
		server.getClient(clientSocket).sendReply("461", ERR_USERINVALID);
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
	if (message.getParameters().size() < 1)
	{
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	Channel *channel = server.getChannel(message.getParameters()[0]);
	if (channel == NULL)
	{
		err_nosuchchannel(server.getClient(clientSocket), message.getParameters()[0]);
		return;
	}
	if (message.getParameters().size() == 1 && message.getText().empty())
	{
		rpl_topic(server.getClient(clientSocket), *channel);
	}
	else if (!message.getText().empty())
	{
		channel->setTopic(message.getText());
		channel->broadcast(server.getClient(clientSocket), " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else if (message.getParameters().size() == 2)
	{
		channel->setTopic(message.getParameters()[1]);
		channel->broadcast(server.getClient(clientSocket), " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}
	else
		server.getClient(clientSocket).sendReply("461", ERR_WRONGPARAMCOUNT);
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

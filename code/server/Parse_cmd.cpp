#include <Server.hpp>

void nick(Server &server, int clientSocket, char *buffer)
{
	char *nickname = strtok(buffer, " ");
	if (nickname == NULL || strlen(nickname) == 0)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	if (server.isNicknameInUse(nickname))
	{
		server.getClient(clientSocket).sendReply("433", ERR_NICKNAMEINUSE);
		return;
	}
	if (strchr(nickname, ' ') != NULL || strchr(nickname, '#') != NULL)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	if (strlen(nickname) > USERLEN)
	{
		server.getClient(clientSocket).sendReply("432", ERR_NICKTOOLONG);
		return;
	}
	server.getClient(clientSocket).setNickname(nickname);
	send(clientSocket, "Nickname successfully changed!\r\n", 34, 0);
}

void user(Server &server, int clientSocket, char *buffer)
{
	char *username = strtok(buffer, " ");
	if (username == NULL || strlen(username) == 0)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	char *nextToken = strtok(NULL, " ");
	if (nextToken == NULL || strcmp(nextToken, "0") != 0)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	nextToken = strtok(NULL, " ");
	if (nextToken == NULL || strcmp(nextToken, "*") != 0)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	char *args = strtok(NULL, " ");
	if (args == NULL)
	{
		server.getClient(clientSocket).sendReply("432", ERR_ERRONEUSNICKNAME);
		return;
	}
	server.getClient(clientSocket).setUsername(username);
	send(clientSocket, "Username successfully changed!\r\n", 34, 0);
}

void oper(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "OPER: " << buffer << std::endl;
}

void mode(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "MODE: " << buffer << std::endl;
}

void quit(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "QUIT: " << buffer << std::endl;
}

void join(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "JOIN: " << buffer << std::endl;
}

void part(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "PART: " << buffer << std::endl;
}

void topic(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "TOPIC: " << buffer << std::endl;
}

void kick(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "KICK: " << buffer << std::endl;
}

void privmsg(Server &server, int clientSocket, char *buffer)
{
	std::string target = strtok(buffer, " ");
	if (target.empty())
	{
		server.getClient(clientSocket).sendReply("411", ERR_NORECIPIENT);
		return;
	}
	std::string message = strtok(NULL, "");
	if (message[0] == ':')
		message = message.substr(1);
	else
	{
		server.getClient(clientSocket).sendReply("412", ERR_NOTEXTTOSEND);
		return;
	}
	if (message.empty())
	{
		server.getClient(clientSocket).sendReply("412", ERR_NOTEXTTOSEND);
		return;
	}
	std::cout << "PRIVMSG: " << target << " " << message << std::endl;

}

void notice(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "clientSocket: " << clientSocket << std::endl;
	std::cout << "NICK: " << server.getClient(clientSocket).getNickname() << std::endl;
	std::cout << "USER: " << server.getClient(clientSocket).getUsername() << std::endl;
}

void sendfile(Server &server, int clientSocket, char *buffer)
{
	(void)server;
	(void)clientSocket;
	(void)buffer;
	std::cout << "SENDFILE: " << buffer << std::endl;
}

void parseCommand(Server &server, int clientSocket, char *buffer)
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
	void (*functions[])(Server &, int, char *) = {
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
	bool commandFound = false;
	while (*buffer && !commandFound)
	{
		if (*buffer == ':')
		{
			buffer++;
			break;
		}
		for (i = 0; i < num_commands; i++)
		{
			if (strncmp(buffer, commands[i], strlen(commands[i])) == 0)
			{
				std::cout << "Command: " << commands[i] << std::endl;
				buffer += strlen(commands[i]);
				functions[i](server, clientSocket, buffer);
				commandFound = true;
				break;
			}
		}
		if (!commandFound)
		{
			server.getClient(clientSocket).sendReply("421", ERR_UNKNOWNCOMMAND);
			break;
		}
	}
}
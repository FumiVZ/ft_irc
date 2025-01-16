#include <Server.hpp>

void nick(Server &server, int clientSocket, char *buffer)
{
	std::cout << "NICK:" << buffer << std::endl;
	if (server.isNicknameInUse(buffer))
	{
		server.getClient(clientSocket).sendReply("433", ERR_NICKNAMEINUSE);
		return;
	}
	server.getClient(clientSocket).setNickname(buffer);
	send(clientSocket, "Nickname sucessfuly changed!\r\n", 31, 0);
	std::cout << "NICK: " << server.getClient(clientSocket).getNickname() << std::endl;
	std::cout << "clientSocket: " << clientSocket << std::endl;
}

void user(Server &server, int clientSocket, char *buffer)
{
	std::cout << "USER:" << buffer << std::endl;
	server.getClient(clientSocket).setUsername(buffer);
	send(clientSocket, "Username sucessfuly changed!\r\n", 31, 0);
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
	(void)server;
	(void)clientSocket;
	(void)buffer;
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
	(void)server;
	(void)clientSocket;
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
	const size_t num_commands = sizeof(commands) / sizeof(commands[0]);
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
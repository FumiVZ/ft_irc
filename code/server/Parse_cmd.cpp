#include <Server.hpp>

/* void pass(Server &server, int clientSocket, char *buffer)
{
	while (*buffer && isspace(*buffer))
		buffer++;
	if (server.authenticateClient(clientSocket, buffer))
	{
		const char *msg = "Authentification réussie\r\n";
		send(clientSocket, msg, strlen(msg), 0);
	}
	else
	{
		const char *msg = "Erreur : Mot de passe incorrect\r\n";
		send(clientSocket, msg, strlen(msg), 0);
	}
} */

void nick(char *buffer)
{
	std::cout << "NICK: " << buffer << std::endl;
}

void user(char *buffer)
{
	std::cout << "USER: " << buffer << std::endl;
}

void oper(char *buffer)
{
	std::cout << "OPER: " << buffer << std::endl;
}

void mode(char *buffer)
{
	std::cout << "MODE: " << buffer << std::endl;
}

void quit(char *buffer)
{
	std::cout << "QUIT: " << buffer << std::endl;
}

void join(char *buffer)
{
	std::cout << "JOIN: " << buffer << std::endl;
}

void part(char *buffer)
{
	std::cout << "PART: " << buffer << std::endl;
}

void topic(char *buffer)
{
	std::cout << "TOPIC: " << buffer << std::endl;
}

void kick(char *buffer)
{
	std::cout << "KICK: " << buffer << std::endl;
}

void privmsg(char *buffer)
{
	std::cout << "PRIVMSG: " << buffer << std::endl;
}

void notice(char *buffer)
{
	std::cout << "NOTICE: " << buffer << std::endl;
}

void sendfile(char *buffer)
{
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
	void (*functions[])(char *) = {
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
				std::cout << "Commande: " << commands[i] << std::endl;
				buffer += strlen(commands[i]);
				functions[i](buffer);
				commandFound = true;
				break;
			}
		}
		if (!commandFound)
		{
			std::cout << "Commande non reconnue" << std::endl;
			break;
		}
	}
}
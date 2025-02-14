#include <Server.hpp>
#include <Commands.hpp>

void upcase(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
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
		client.sendReply("451", (((client.getNickname().empty() ? "* " : client.getUsername()))) + " :" + ERR_NOTREGISTERED);
		return;
	}
	client.sendReply("421", client.getNickname() + " " + message.getCommand() + " :" + ERR_UNKNOWNCOMMAND);
}

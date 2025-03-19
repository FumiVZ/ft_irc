#include <Commands.hpp>

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
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 1)
	{
		client.sendReply("431",(client.getNickname().empty() ? "* " : client.getUsername()) + " :" + ERR_NONICKNAMEGIVEN);
		return;
	}
	std::string nickname = message.getParameters()[0];
	if (nickname.empty())
	{
		client.sendReply("431",(client.getNickname().empty() ? "* " : client.getUsername()) + " :" + ERR_NONICKNAMEGIVEN);
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
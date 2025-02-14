#include "Commands.hpp"

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
	Client &client = server.getClient(clientSocket);
	if (!client.getUsername().empty())
	{
		client.sendReply("462",(client.getNickname().empty() ? "* " : client.getUsername()) + " :" + ERR_ALREADYREGISTERED);
		return;
	}
	if (message.getParameters().size() != 3)
	{
		client.sendReply("461",(client.getNickname().empty() ? "* " : client.getUsername()) + " USER :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	std::vector<std::string> parameters = message.getParameters();
	std::string username = parameters[0];
	if (username.empty())
	{
		client.sendReply("421",(client.getNickname().empty() ? "* " : client.getUsername()) + " :" + ERR_UNKNOWNCOMMAND);
		return;
	}
	if (!is_valid_username(username))
	{
		client.sendReply("401",(client.getNickname().empty() ? "* " : client.getUsername()) + " " + username + " :" + ERR_USERINVALID);
		return;
	}
	client.setUsername(username);
	if (!client.getNickname().empty())
	{
		rpl_welcome(client);
		rpl_motd(clientSocket);
	}
}
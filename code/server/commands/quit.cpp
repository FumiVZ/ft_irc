#include <Commands.hpp>

void quit(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	if (message.getParameters().size() != 0)
	{
		client.sendReply("461", client.getNickname().empty() ? "* " : client.getUsername() + " QUIT :" + ERR_WRONGPARAMCOUNT);
		return;
	}
	client.setAuth(false);
	throw client.disconnected;
}
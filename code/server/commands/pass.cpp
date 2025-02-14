#include <Commands.hpp>

void pass_auth(Server &server, int clientSocket, Message message)
{
	Client &client = server.getClient(clientSocket);
	(void)message;
	std::cout << "pass auth" << std::endl;
	client.sendReply("462",(client.getNickname().empty() ? "* " : client.getUsername()) + " :" + ERR_ALREADYREGISTERED);
}
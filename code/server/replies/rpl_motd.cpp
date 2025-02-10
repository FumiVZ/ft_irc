#include <fstream>
#include <replies.hpp>

void rpl_motd(Server &server, int client_socket, Message message)
{
	std::cout << "Param size: " << message.getParameters().size() << std::endl;
	if (message.getParameters().size() > 1)
	{
		server.getClient(client_socket).sendReply("461", ERR_WRONGPARAMCOUNT);
		return;
	}
	Client &client = server.getClient(client_socket);
	std::ifstream file("motd.txt");
	if (message.getParameters().size() == 1)
	{
		Client &target = server.getClient(message.getParameters()[0]);
		client = target;
	}
	if (!file.is_open())
	{
		client.sendReply("422", "MOTD File is missing");
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		client.forwardMessage(line + "\r\n");
	}
	file.close();
}
#include <Server.hpp>
#include <Client.hpp>
#include <Channel.hpp>
#include <fstream>

void rpl_motd(Client &client)
{
	std::ifstream file("motd.txt");

	if (!file.is_open())
	{
		client.sendReply("422", "MOTD File is missing");
		return;
	}
	std::string line;
	while (std::getline(file, line))
		client.sendReply("375", line);
	file.close();
}
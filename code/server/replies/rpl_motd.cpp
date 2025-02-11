#include <fstream>
#include <replies.hpp>

void rpl_motd(int clientSocket)
{
	std::ifstream file("motd.txt");
	std::string line;
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			send(clientSocket, line.c_str(), line.size(), 0);
			send(clientSocket, "\r\n", 2, 0);
		}
		file.close();
	}
	else
		send(clientSocket, "No MOTD file found\r\n", 20, 0);
}
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include "Wordle.hpp"

void parseMessage(const std::string &message, std::string &sender, std::string &command, std::string &channel, std::string &content)
{
	size_t pos = message.find(" ");
	if (pos != std::string::npos)
	{
		sender = message.substr(1, pos - 1);
		size_t commandPos = message.find(" ", pos + 1);
		if (commandPos != std::string::npos)
		{
			command = message.substr(pos + 1, commandPos - pos - 1);
			size_t channelPos = message.find(" :", commandPos + 1);
			if (channelPos != std::string::npos)
			{
				channel = message.substr(commandPos + 1, channelPos - commandPos - 1);
				content = message.substr(channelPos + 2);
			}
		}
	}
}

void startWordle(int sock, const std::string &channel)
{
	Wordle wordleGame;
	std::string word;
	std::string result;
	while (!wordleGame.CheckWin(word))
	{
		std::string message = "PRIVMSG " + channel + " :";
		send(sock, message.c_str(), message.length(), 0);
		std::cin >> word;
		result = wordleGame.playWordle(word);
		message = "PRIVMSG " + channel + " :" + result + "\r\n";
		send(sock, message.c_str(), message.length(), 0);
	}
	std::string message = "PRIVMSG " + channel + " :You won!" + "\r\n";
	send(sock, message.c_str(), message.length(), 0);
}

int main(int ac, char **av)
{
	if (ac < 4)
	{
		std::cerr << "Usage: " << av[0] << " <password> <port> <ip>" << std::endl;
		return 1;
	}
	std::istringstream port(av[2]);
	int port_int;
	port >> port_int;
	if (port_int <= 0 || port_int > 65535)
	{
		std::cerr << "Port must be greater than 0 or inferior to 65535" << std::endl;
		return 1;
	}
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		std::cerr << "Erreur création socket" << std::endl;
		return 1;
	}
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_int);
	serverAddr.sin_addr.s_addr = inet_addr(av[3]);

	if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Erreur connexion" << std::endl;
		close(sock);
		return 1;
	}

	std::cout << "Connecté au serveur IRC local" << std::endl;

	const char *password = av[1];
	const char *nickname = "MonBot";
	const char *user = "monbot 0 * :monbot\r\n";
	std::string pass = "PASS " + std::string(password) + "\r\n";
	std::string nick = "NICK " + std::string(nickname) + "\r\n";
	std::string usr = "USER " + std::string(user) + "\r\n";
	send(sock, pass.c_str(), pass.length(), 0);
	usleep(100000); // 100ms delay
	send(sock, nick.c_str(), nick.length(), 0);
	usleep(100000); // 100ms delay
	send(sock, usr.c_str(), usr.length(), 0);
	
	// usleep(100000); // 100ms delay
	// send(sock, user.c_str(), user.length(), 0);

	char buffer[1024];
	bool connected = false;
	while (!connected)
	{
		memset(buffer, 0, sizeof(buffer));
		if (recv(sock, buffer, sizeof(buffer), 0) > 0)
		{
			if (strstr(buffer, "001") != NULL)
			{
				std::cout << "Connecté avec succès!" << std::endl;
				connected = true;
			}
		}
		usleep(100000); // 100ms delay
	}

	std::string join = "JOIN #channel\r\n";
	send(sock, join.c_str(), join.length(), 0);
	usleep(500000);

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead <= 0)
			break;

		std::string message(buffer);
		std::string sender, command, channel, content;
		parseMessage(message, sender, command, channel, content);

		if (command == "PRIVMSG")
		{
			if (content.substr(0, 7) == "!wordle ")
			{
				startWordle(sock, channel);
			}
		}
	}
	close(sock);
	return 0;
}

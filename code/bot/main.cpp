#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "Wordle.hpp"

static int g_signal = 1;

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

void sigint_handler(int signum)
{
	if (signum == SIGINT)
		g_signal = 0;
	std::cout << "Server stopped" << std::endl;
}

std::string MakeVisible(std::string str)
{
	std::string result;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '\0')
			result += "EOF";
		else if (str[i] == '\a')
			result += "\\a";
		else if (str[i] == '\b')
			result += "\\b";
		else if (str[i] == '\t')
			result += "\\t";
		else if (str[i] == '\n')
			result += "\\n";
		else if (str[i] == '\v')
			result += "\\v";
		else if (str[i] == '\f')
			result += "\\f";
		else if (str[i] == '\r')
			result += "\\r";
		else
			result += str[i];
	}
	return result;
}

int main(int ac, char **av)
{
	if (ac < 4)
	{
		std::cerr << "Usage: " << av[0] << " <password> <port> <ip>" << std::endl;
		return 1;
	}
	signal(SIGINT, sigint_handler);
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
	usleep(50000);
	send(sock, nick.c_str(), nick.length(), 0);
	usleep(50000);
	send(sock, usr.c_str(), usr.length(), 0);
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
		usleep(50000);
	}
	std::string join = "JOIN #channel\r\n";
	send(sock, join.c_str(), join.length(), 0);
	usleep(50000);
	Wordle wordleGame;
	bool gameStarted = false;
	std::string result;
	while (g_signal)
	{
		memset(buffer, 0, sizeof(buffer));
		usleep(50000);
		fcntl(sock, F_SETFL, O_NONBLOCK);
		int bytesRead = recv(sock, buffer, sizeof(buffer), 0);
		if (bytesRead < 0)
			continue;
		else if (bytesRead == 0)
		{
			std::cerr << "Connection closed" << std::endl;
			break;
		}
		std::string message(buffer, bytesRead);
		std::string sender, command, channel, content;
		content = "";
		parseMessage(message, sender, command, channel, content);
		memset(buffer, 0, sizeof(buffer));
		if (command == "PRIVMSG")
		{
			if (content.substr(0, 7) == "!wordle" || gameStarted)
			{
				if (!gameStarted)
				{
					send(sock, "PRIVMSG #CHANNEL :Starting Wordle game\r\n", 41, 0);
					usleep(50000);
					gameStarted = true;
					continue;
				}
				result = wordleGame.playWordle(content);
				std::string msg;
				msg = "PRIVMSG #CHANNEL :" + result + "\r\n";
				if (result == "YOU WIN!")
				{
					send(sock, "PRIVMSG #CHANNEL :You win!\r\n", 29, 0);
					usleep(50000);
					gameStarted = false;
				}
				else
				{
					size_t i;
					for (i = 0; i < msg.length(); i++)
						if (msg[i] == '\0')
							msg.erase(i, 1);
					i = 0;
					std::cout << "Sending: " << MakeVisible(msg) << std::endl;
					std::cout << "SOCKET: " << sock << std::endl;
					std::cout << "LENGTH: " << msg.length() << std::endl;
					send(sock, msg.c_str(), msg.length(), 0);
				}

			}
		}
	}
	close(sock);
	return 0;
}

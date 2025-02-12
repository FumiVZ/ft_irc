#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <Server.hpp>

void pass_auth(Server &server, int clientSocket, Message message);
void nick(Server &server, int clientSocket, Message message);
void user(Server &server, int clientSocket, Message message);
void mode(Server &server, int clientSocket, Message message);
void quit(Server &server, int clientSocket, Message message);
void join(Server &server, int clientSocket, Message message);
void topic(Server &server, int clientSocket, Message message);
void kick(Server &server, int clientSocket, Message message);
void privmsg(Server &server, int clientSocket, Message message);
void invite(Server &server, int clientSocket, Message message);

#endif
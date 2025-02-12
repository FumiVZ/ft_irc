#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <Client.hpp>
#include <Channel.hpp>
#include <Server.hpp>

class Server;

#define ERR_WRONGPARAM "Wrong usage use: ./ircserv <port> <password>"
#define ERR_PASSWDMISMATCH "Password incorrect use PASS <password>"
#define ERR_NICKNAMEINUSE "This nickname is already in use"
#define ERR_USERINVALID "Invalid username"
#define ERR_UNKNOWNCOMMAND "Unknown command"
#define ERR_ERRONEUSNICKNAME "Erroneus nickname"
#define ERR_NOTEXTTOSEND "No text to send"
#define ERR_NORECIPIENT "No target given"
#define ERR_NONICKNAMEGIVEN " :No nickname given"
#define ERR_NOSUCHNICK "No such nick/channel"
#define ERR_WRONGPARAMCOUNT "Wrong number of parameters"
#define ERR_NOTREGISTERED "You have not registered"
#define ERR_ALREADYREGISTERED "You may not reregister"
#define ERR_CHANOPRIVSNEEDED "You're not a channel operator"
#define ERR_NOTONCHANNEL "You're not on that channel"
#define ERR_NOSUCHCHANNEL "No such channel"
#define ERR_USERNOTINCHANNEL "User not in channel"
#define ERR_CHANNELISFULL "Channel is full"
#define ERR_BADCHANNELKEY "Bad channel key"
#define ERR_UNKNOWNMODE "Unknown mode"
#define ERR_INVITEONLYCHAN "Invite only channel"
#define ERR_MESSAGETOOLONG "Message too long"
#define ERR_INPUTTOOLONG "Input line was too long"
void rpl_welcome(Client client);
void rpl_endofnames(Client &c);
void rpl_namreply(Client &c, Channel &ch);
void rpl_topic(Client &c, Channel &ch);
void rpl_invite(Client &requester, Client &target, Channel *channel);
void rpl_motd(int clientSocket);

void rpl_needmoreparam(Client c, std::string command);

#endif
#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <Client.hpp>
#define ERR_WRONGPARAM "Wrong usage use: ./ircserv <port> <password>"
#define ERR_PASSWDMISMATCH "Password incorrect use PASS <password>"
#define ERR_NICKNAMEINUSE "This nickname is already in use"
#define ERR_NICKNAMEINVALID "This nickname is invalid"
#define ERR_NICKNAMEUNSET "You must set a nickname (NICK <nickname>)"
#define ERR_NICKNAMECHANGED "Your nickname has been changed"
#define ERR_NICKNAME "Invalid nickname"
#define ERR_USERINVALID "Invalid username"
#define ERR_UNKNOWNCOMMAND "Unknown command"
#define ERR_USERNAMEUNSET "You must set a username (USER <username> * 8 <:realname>)"
#define ERR_ERRONEUSNICKNAME "Erroneus nickname"
#define ERR_NICKTOOLONG "Nickname too long"
#define ERR_NICKCOLLISION "Nickname collision"
#define ERR_NICKRESERVED "Nickname reserved"
#define ERR_NOTEXTTOSEND "No text to send"
#define ERR_NORECIPIENT "No target given"
#define ERR_NONICKNAMEGIVEN "No nickname given"
#define ERR_NOSUCHNICK "No such nick/channel"
#define ERR_WRONGPARAMCOUNT "Wrong number of parameters"
#define ERR_NOTREGISTERED "You have not registered"
#define ERR_ALREADYREGISTERED "You may not reregister"
#define ERR_CHANOPRIVSNEEDED "You're not a channel operator"
#define ERR_NOTONCHANNEL "You're not on that channel"
#define ERR_NEEDMOREPARAMS "Not enough parameters"
#define ERR_NOSUCHCHANNEL "No such channel"
#define ERR_USERNOTINCHANNEL "User not in channel"
#define ERR_USERSDONTMATCH "Users don't match"
#define ERR_CHANNELISFULL "Channel is full"
#define ERR_BADCHANNELKEY "Bad channel key"
#define ERR_UNKNOWNMODE "Unknown mode"
#define ERR_INVITEONLYCHAN "Invite only channel"
void rpl_welcome(Client client);
void rpl_endofnames(Client &c);
void rpl_namreply(Client &c, Channel &ch);
void rpl_topic(Client &c, Channel &ch);
void rpl_invite(Client &requester, Client &target, Channel *channel);
void rpl_motd(Client &client);

void err_nosuchnick(Client c, std::string nickname);
void err_nosuchchannel(Client c, std::string channel);
void err_erroneusnickname(Client c, std::string nickname);
void rpl_needmoreparam(Client c, std::string command);

#endif
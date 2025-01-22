#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <Client.hpp>
#define ERR_PASSWDMISMATCH "Password incorrect use PASS <password>"
#define ERR_NICKNAMEINUSE "This nickname is already in use"
#define ERR_NICKNAMEINVALID "This nickname is invalid"
#define ERR_NICKNAMEUNSET "You must set a nickname (NICK <nickname>)"
#define ERR_NICKNAMECHANGED "Your nickname has been changed"
#define ERR_NICKNAME "Invalid nickname"
#define ERR_USERINVALID "Invalid username"
#define ERR_UNKNOWNCOMMAND "Unknown command"
#define ERR_USERNAMEUNSET "You must set a username (USER <username>)"
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
void rpl_welcome(Client client);
#endif
#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <Client.hpp>
#define ERR_PASSWDMISMATCH "Password incorrect use PASS <password>"
#define ERR_NICKNAMEINUSE "This nickname is already in use"
#define ERR_NICKNAMEINVALID "This nickname is invalid"
#define ERR_NICKNAMEUNSET "You must set a nickname"
#define ERR_NICKNAMECHANGED "Your nickname has been changed"
#define ERR_NICKNAME "Invalid nickname"
#define ERR_UNKNOWNCOMMAND "Unknown command"

void rpl_welcome(Client client);
#endif
#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <Client.hpp>

void rpl_welcome(Client client);
void rpl_endofnames(Client c, std::string server_hostname);
void rpl_namreply(Client c, Channel ch, std::string server_hostname);
#endif
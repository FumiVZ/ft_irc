#include <Client.hpp>
#include <Server.hpp>

void rpl_endofnames(Client c)
{
	std::string reply = ":" + get_g_hostname() + " 366 " + "End of /NAMES list." + "\r\n";
}
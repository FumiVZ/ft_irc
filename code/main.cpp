#include <Server.hpp>
#include <Client.hpp>
#include <signal.h>

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << ERR_WRONGPARAM << std::endl;
		return 1;
	}
	server(av[1], av[2]);
	return 0;
}
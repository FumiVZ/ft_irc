#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>

class Channel
{
private:
	Client &owner;
public:
	Channel(/* args */);
	~Channel();
};


#endif
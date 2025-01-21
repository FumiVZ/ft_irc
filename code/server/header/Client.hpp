#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include <vector>
#include <string>

#define USERLEN 12

class Channel;
class Client
{
private:
	std::string nickname;
	std::string username;
	std::string ip;
	std::string hostname;
	std::string full_client_id;
	std::vector<Channel> channels;
	int socketfd;
	bool is_auth;

public:
	Client(int socketfd, std::string ip, std::string hostname);
	Client();
	~Client();

	void setIp(std::string ip);
	std::string getIp();
	void setSocketfd(int socketfd);
	int getSocketfd() const;
	void setAuth(bool auth);
	bool getAuth();
	std::string getNickname();
	void setNickname(std::string nickname);
	std::string getUsername();
	void setUsername(std::string username);
	std::string getHostname();
	void setHostname(std::string hostname);

	void sendReply(std::string code, std::string message);
	bool isAuthentified();
	void setAuthentified();
	char mode;
	/*
	a - user is flagged as away;
    i - marks a users as invisible;
    w - user receives wallops;
    r - restricted user connection;
    o - operator flag;
    O - local operator flag;
    s - marks a user for receipt of server notices.
	*/
};

#endif
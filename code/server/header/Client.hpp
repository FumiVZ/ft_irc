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
	std::vector<Channel *> channels;
	std::string buffer;
	int socketfd;
	bool is_auth;
	bool hex_chat;
public:
	Client(int socketfd, std::string ip, std::string hostname);
	~Client();

	void setIp(std::string ip);
	std::string getIp();
	void setSocketfd(int socketfd);
	int getSocketfd() const;
	void setAuth(bool auth);
	bool getAuth();
	void setHexChat(bool hex_chat);
	bool getHexChat();
	std::string getNickname();
	void setNickname(std::string nickname);
	std::string getUsername();
	void setUsername(std::string username);
	std::string getHostname();
	void setHostname(std::string hostname);
	bool isAuthentified();
	void setAuthentified();
	bool isNamed();
	void addChannel(Channel *channel);
	void removeChannel(Channel *channel);
	std::vector<Channel *> &getChannels();
	void setBuffer(std::string buffer);
	std::string getBuffer();

	void forwardMessage(std::string message);
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
	bool operator==(const Client &c) const;
	void sendMsg(std::string msg, Channel &ch);
	void sendMsg(std::string msg, Client &c);
	void sendReply(std::string code, std::string message);
};

#endif
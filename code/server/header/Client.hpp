#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include <vector>
#include <string>
class Client
{
private:
	std::string nickname;
	std::string username;
	std::string ip;
	std::string hostname;
	std::string full_client_id;
	bool is_auth;
	std::vector<Channel> channels;
	int socketfd;

public:
	Client(int socketfd, std::string ip, std::string hostname);
	Client();
	~Client();

	void setIp(std::string ip);
	std::string getIp();
	void setSocketfd(int socketfd);
	int getSocketfd();
	void setAuth(bool auth);
	bool getAuth();
	std::string getNickname();
	void setNickname(std::string nickname);
	std::string getUsername();
	void setUsername(std::string username);
	std::string getHostname();
	void setHostname(std::string hostname);

	void sendReply(std::string code, std::string message);
};



#endif
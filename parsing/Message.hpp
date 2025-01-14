#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

#include <string>
#include <vector>
#include <stdexcept>

#define MAX_MESSAGE_SIZE 512

class Message
{
private:
	std::string _raw_message;
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _parameters;
	int _nb_parameters;
	std::string _text;

	void parse();
	bool isValidMessage() const;

public:
	explicit Message(const std::string &message);

	const std::string &getPrefix() const;
	const std::string &getCommand() const;
	const std::vector<std::string> &getParameters() const;
	const std::string &getRawMessage() const;
	const std::string &getText() const;
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

#endif
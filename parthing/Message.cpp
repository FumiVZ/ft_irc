#include "Message.hpp"
#include <sstream>
#include <iostream>

Message::Message(const std::string &message) : _raw_message(message)
{
	if (message.empty())
	{
		throw std::invalid_argument("Empty message");
	}
	if (message.length() < 2)
	{
		throw std::invalid_argument("Message too short");
	}
	if (message.length() > MAX_MESSAGE_SIZE - 2)
	{
		throw std::invalid_argument("Message too long");
	}
	if (message[0] == ' ')
	{
		throw std::invalid_argument("Message starts with a space");
	}
	// if (message[message.length() - 1] != '\n')
	// {
	// 	throw std::invalid_argument("Message does not end with a newline");
	// }
	// if (message[message.length() - 2] != '\r')
	// {
	// 	throw std::invalid_argument("Message does not end with a carriage return");
	// }
	parse();
}

void Message::parse()
{
	std::string msg = _raw_message;
	size_t pos = 0;

	if (!msg.empty() && msg[0] == ':')
	{
		pos = msg.find(' ');
		if (pos != std::string::npos)
		{
			_prefix = msg.substr(1, pos - 1);
			msg = msg.substr(pos + 1);
		}
	}

	if (pos == std::string::npos)
	{
		throw std::invalid_argument("No command found");
	}

	pos = msg.find(' ');
	if (pos != std::string::npos)
	{
		_command = msg.substr(0, pos);
		msg = msg.substr(pos + 1);
	}
	else
	{
		_command = msg;
	}

	for (char c : _command)
	{
		if (!std::isupper(c))
		{
			throw std::invalid_argument("Command is not entirely uppercase");
		}
	}

	if (pos == std::string::npos)
	{
		return;
	}

	while (!msg.empty())
	{
		if (msg[0] == ':')
		{
			_parameters.push_back(msg.substr(1));
			break;
		}

		pos = msg.find(' ');
		if (pos != std::string::npos)
		{
			_parameters.push_back(msg.substr(0, pos));
			msg = msg.substr(pos + 1);
		}
		else
		{
			_parameters.push_back(msg);
			break;
		}
	}

	if (!isValidMessage())
	{
		throw std::invalid_argument("Invalid IRC message format");
	}
}

bool Message::isValidMessage() const
{
	if (_command.empty())
		return false;
	if (_parameters.size() > 15)
		return false;
	return true;
}

const std::string &Message::getPrefix() const
{
	return _prefix;
}

const std::string &Message::getCommand() const
{
	return _command;
}

const std::vector<std::string> &Message::getParameters() const
{
	return _parameters;
}

const std::string &Message::getRawMessage() const
{
	return _raw_message;
}

std::ostream &operator<<(std::ostream &os, const Message &msg)
{
	os << "Raw Message: " << msg.getRawMessage() << std::endl;
	os << "Prefix: " << msg.getPrefix() << std::endl;
	os << "Command: " << msg.getCommand() << std::endl;
	os << "Parameters: ";
	for (const auto &param : msg.getParameters())
		os << param << " ";
	os << std::endl;
	return os;
}
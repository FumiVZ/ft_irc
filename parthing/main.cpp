#include <iostream>
#include <cassert>
#include "Message.hpp"

void testValidMessage()
{
	std::string raw_msg = ":prefix COMMAND param1 param2 :trailing param\r\n";
	Message msg(raw_msg);

	std::cout << msg << std::endl;
	assert(msg.getPrefix() == "prefix");
	assert(msg.getCommand() == "COMMAND");
	assert(msg.getParameters().size() == 2);
	assert(msg.getParameters()[0] == "param1");
	assert(msg.getParameters()[1] == "param2");
	assert(msg.getText() == "trailing param");
	assert(msg.getRawMessage() == raw_msg);
}

void testNoPrefix()
{
	std::string raw_msg = "COMMAND param1 param2 :trailing param\r\n";
	Message msg(raw_msg);

	std::cout << "Testing message with no prefix..." << std::endl;
	std::cout << msg << std::endl;

	assert(msg.getPrefix().empty());
	assert(msg.getCommand() == "COMMAND");
	assert(msg.getParameters().size() == 2);
	assert(msg.getParameters()[0] == "param1");
	assert(msg.getParameters()[1] == "param2");
	assert(msg.getText() == "trailing param");
	assert(msg.getRawMessage() == raw_msg);
}

void testNoParameters()
{
	std::string raw_msg = ":prefix COMMAND\r\n";
	Message msg(raw_msg);

	std::cout << "Testing message with no parameters..." << std::endl;
	std::cout << "Prefix: " << msg.getPrefix() << std::endl;
	std::cout << "Command: " << msg.getCommand() << std::endl;
	std::cout << "Parameters: ";
	for (const auto &param : msg.getParameters())
		std::cout << param << " ";
	std::cout << std::endl;
	std::cout << "Raw Message: " << msg.getRawMessage() << std::endl;

	assert(msg.getPrefix() == "prefix");
	assert(msg.getCommand() == "COMMAND");
	assert(msg.getParameters().empty());
	assert(msg.getRawMessage() == raw_msg);
}

void testInvalidMessage()
{
	try
	{
		std::string raw_msg = ":prefix\r\n";
		Message msg(raw_msg);
		std::cout << "Testing invalid message..." << std::endl;
		std::cout << msg << std::endl;
		assert(false); // Exception should be thrown
	}
	catch (const std::invalid_argument &e)
	{
		std::cout << "Testing invalid message..." << std::endl;
		std::cout << "Exception caught: " << e.what() << std::endl;
		assert(true); // Exception should be thrown
	}
}

void testMaxLength()
{
	std::string long_msg(510, 'A'); // 510 characters
	Message msg(long_msg + "\r\n");

	std::cout << "Testing max length message..." << std::endl;
	std::cout << "Raw Message Length: " << msg.getRawMessage().length() << std::endl;

	assert(msg.getRawMessage().length() == 512); // 510 + \r\n
	std::cout << msg << std::endl;
}

int main()
{
	testValidMessage();
	testNoPrefix();
	testNoParameters();
	testInvalidMessage();
	testMaxLength();

	std::cout << "All tests passed!" << std::endl;
	return 0;
}
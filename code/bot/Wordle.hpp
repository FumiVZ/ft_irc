#ifndef WORDLE_HPP
#define WORDLE_HPP

#include <string>
#include <vector>

class Wordle
{
public:
	Wordle();
	bool CheckWin(std::string &word, std::string result);
	std::string playWordle(std::string &word);
	~Wordle();
	std::string getWord() { return this->Word; }

private:
	std::vector<std::string> wordbase;
	std::string Word;
	void populateVector();
	std::string	randomword();
};

#endif
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>
#include "Wordle.hpp"

Wordle::Wordle()
{
	std::srand(static_cast<unsigned int>(std::time(0)));
	populateVector();
	this->Word = randomword();
	std::cout << "Word to guess: " << this->Word << std::endl;
}

Wordle::~Wordle()
{
}

void Wordle::populateVector()
{
	std::string line;
	std::ifstream myfile("words.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			wordbase.push_back(line);
		}
		myfile.close();
	}
	else
	{
		throw std::runtime_error("Unable to open words.txt");
	}

	if (wordbase.empty())
	{
		throw std::runtime_error("No words loaded from file");
	}
}

std::string Wordle::randomword()
{
	std::random_shuffle(wordbase.begin(), wordbase.end());
	std::cout << "Word to guess: " << wordbase[0] << std::endl;
	return wordbase[0];
}

std::string Wordle::playWordle(std::string &word)
{
	std::string result = "";
	for (int i = 0; i < 5; i++)
	{
		if (word[i] == this->Word[i])
		{
			result += "\033[32m" + std::string(1, word[i]) + "\033[0m";
		}
		else if (this->Word.find(word[i]) != std::string::npos)
		{
			result += "\033[33m" + std::string(1, word[i]) + "\033[0m";
		}
		else
		{
			result += word[i];
		}
	}
	if (word.substr(0, word.find('\r')) == this->Word){
		result = "YOU WIN!";
		this->Word = randomword();
	}
	return result;
}



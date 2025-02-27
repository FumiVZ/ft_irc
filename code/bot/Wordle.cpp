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
	if (word.substr(0, word.find('\r')) == this->Word)
	{
		result = "YOU WIN!";
		this->Word = randomword();
		return result;
	}
	for (int i = 0; i < 5; i++)
	{
		if (word[i] == this->Word[i])
		{
			result += "\033[32m" + std::string(1, word[i]) + "\033[0m";
		}
		else
		{
			int targetCount = 0;
			for (size_t k = 0; k < this->Word.length(); k++)
			{
				if (this->Word[k] == word[i])
					targetCount++;
			}

			int usedCount = 0;
			for (int j = 0; j < i; j++)
			{
				if (word[j] == word[i] &&
					(word[j] == this->Word[j] ||
					 this->Word.find(word[j]) != std::string::npos))
				{
					usedCount++;
				}
			}

			bool letterFound = false;
			for (size_t m = 0; m < this->Word.length(); m++)
			{
				if (this->Word[m] == word[i])
				{
					letterFound = true;
					break;
				}
			}

			if (letterFound && usedCount < targetCount)
			{
				result += "\033[33m" + std::string(1, word[i]) + "\033[0m";
			}
			else
			{
				result += word[i];
			}
		}
	}
	return result;
}

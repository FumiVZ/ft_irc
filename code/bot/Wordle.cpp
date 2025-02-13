#include <stdio.h>
#include <fstream>
#include <iostream>

#include <algorithm>
#include "Wordle.hpp"

Wordle::Wordle()
{
    populateVector();
    this->Word = randomword();
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
    return result;
}

bool Wordle::CheckWin(std::string &word)
{
    if (word == this->Word)
    {
        return true;
    }
    return false;
}
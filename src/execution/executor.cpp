#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "execution/executor.hpp"

std::string Execute(Tree& tree, std::vector<std::string> commands)
{
    std::string result = "";
    for(size_t i = 0; i < commands.size(); i++)
    {
        std::string command = commands[i];
        if(command == "k")
        {
            int value = std::stoi(commands[++i]);
            tree.Insert(value);
        }
        else if(command == "q")
        {
            int l_value = std::stoi(commands[++i]);
            int r_value = std::stoi(commands[++i]);
            result += tree.Keys_in_interval(l_value, r_value) + " ";
        }
        else if(command == "m")
        {
            int index = std::stoi(commands[++i]);
            result += tree.Keys_small_number(index) + " ";
        }
        else if(command == "n")
        {
            int x = std::stoi(commands[++i]);
            result += tree.Key_smaller(x) + " ";
        }
        else 
        {
            std::cout << "Invalid command" << std::endl;
            return "";
        }
    }

    return result;
}

std::vector<std::string> Parse(std::string input)
{
    std::istringstream stream(input);
    std::string word;
    std::vector<std::string> words;

    while (stream >> word) words.push_back(word);

    return words;
}
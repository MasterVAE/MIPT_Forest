#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "execution/executor.hpp"


std::string Execute(Tree& tree, std::vector<std::string> commands)
{
    std::string result = "";

    for (size_t i = 0; i < commands.size(); i++)
    {
        std::string command = commands[i];

        if (command == "k")
        {
            if (i + 1 >= commands.size())
            {
                std::cerr << "Error: k requires an argument" << std::endl;
                return "";
            }

            int value = std::stoi(commands[++i]);
            tree.Insert(value);
        }
        else if (command == "q")
        {
            if (i + 2 >= commands.size())
            {
                std::cerr << "Error: q requires two arguments" << std::endl;
                return "";
            }

            int l_value = std::stoi(commands[++i]);
            int r_value = std::stoi(commands[++i]);

            result += std::to_string(tree.Keys_in_interval(l_value, r_value));

            result += " ";
        }
        else if (command == "m")
        {
            if (i + 1 >= commands.size())
            {
                std::cerr << "Error: m requires an argument" << std::endl;
                return "";
            }

            int index = std::stoi(commands[++i]);

            result += std::to_string(tree.Keys_small_number(index));

            result += " ";
        }
        else if (command == "n")
        {
            if (i + 1 >= commands.size())
            {
                std::cerr << "Error: n requires an argument" << std::endl;
                return "";
            }

            int x = std::stoi(commands[++i]);

            result += std::to_string(tree.Key_smaller(x));

            result += " ";
        }
        else
        {
            std::cerr << "Invalid command: " << command << std::endl;
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

    while (stream >> word)
    {
        words.push_back(word);
    }

    return words;
}

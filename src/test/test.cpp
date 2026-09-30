#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tree/tree.hpp"
#include "execution/executor.hpp"

static std::vector<std::string> Tokenize(const std::string& s)
{
    std::istringstream iss(s);
    std::vector<std::string> tokens;
    std::string t;
    while (iss >> t) tokens.push_back(t);
    return tokens;
}

void RunTests(Tree& tree)
{
    std::ifstream file("resources/test.tst");
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        std::cerr << "Error: empty test file" << std::endl;
        return;
    }
    int test_count = std::stoi(line);

    bool all_passed = true;

    for (int i = 0; i < test_count; ++i)
    {
        tree.Clear();

        std::string commands_line, answer_line;
        if (!std::getline(file, commands_line) ||
            !std::getline(file, answer_line))
        {
            std::cerr << "Error: unexpected end of file on test "
                      << (i + 1) << std::endl;
            return;
        }

        std::string result = Execute(tree, Parse(commands_line));

        auto expected = Tokenize(answer_line);
        auto got      = Tokenize(result);

        if (expected != got)
        {
            all_passed = false;
            std::cout << "-- Test " << (i + 1) << " failed --\n";
            std::cout << "   input:    " << commands_line << "\n";
            std::cout << "   expected: " << answer_line << "\n";
            std::cout << "   got:      " << result << "\n";
        }
        else
        {
            std::cout << "-- Test " << (i + 1) << " passed --\n";
        }
    }

    std::cout << (all_passed ? "== All tests passed ==\n"
                             : "== Tests failed ==\n");
}
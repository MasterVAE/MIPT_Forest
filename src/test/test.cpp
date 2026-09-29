#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tree/tree.hpp"
#include "test/test.hpp"
#include "execution/executor.hpp"

void RunTests(Tree& tree)
{
    std::ifstream file("resources/test.tst");

    if (!file.is_open()) 
    {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);
    int test_count = std::stoi(line);

    bool pass = true;

    for(int i = 0; i < test_count; i++)
    {
        std::string answer;
        std::getline(file, line);
        std::getline(file, answer);

        std::string result = Execute(tree, Parse(line));

        if(result != answer)
        {
            pass = false;
            std::cout << "-- Test " << i << " failed --" << std::endl;
        }
        else
        {
            std::cout << "-- Test " << i << " passed --" << std::endl;
        }
    }

    if(pass)
    {
        std::cout << "== All test passed ==" << std::endl;
    }
    else
    {
        std::cout << "== Test failed == " << std::endl;
    }

    file.close();
}
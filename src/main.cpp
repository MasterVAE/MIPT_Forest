#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <sstream>

#include "tree/tree.hpp"
#include "execution/executor.hpp"
#include "test/test.hpp"

namespace
{
    std::unique_ptr<Tree> MakeTree(const std::string& kind)
    {
        if (kind == "set")  return std::make_unique<Tree_set>();
        if (kind == "vec")  return std::make_unique<Tree_vec>();
        if (kind == "nat")  return std::make_unique<Tree_nat>();
        return std::make_unique<Tree_nat>();
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) return 0;
    auto tree = MakeTree(argv[1]);

    if(argc > 2 && (std::string)argv[2] == "test") RunTests(*tree);
    else 
    {
        std::string user_input;
        std::getline(std::cin, user_input); 

        std::cout << Execute(*tree, Parse(user_input)) << std::endl;
    }
}
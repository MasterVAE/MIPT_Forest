#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <string>
#include <vector>

#include "tree/tree.hpp"

std::string Execute(Tree&, std::vector<std::string>);

std::vector<std::string> Parse(std::string);

#endif // EXECUTOR_HPP
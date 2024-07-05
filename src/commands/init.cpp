#include <init.h>
#include <iostream>
#include <vector>
#include <string>

void init_command(const std::vector<std::string>& args) {
    std::cout << "In init and currently changed\n";
    for(auto const &str : args) std::cout << str << std::endl;
}
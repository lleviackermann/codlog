#ifndef ADD_H
#define ADD_H

#include <vector>
#include <string>

void add_command(const std::vector<std::string>& args, std::string& initialized_repo);
void check_add_arguments(const std::vector<std::string>& args);

#endif
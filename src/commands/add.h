#ifndef ADD_H
#define ADD_H

#include <filemode.h>
#include <vector>
#include <string>

struct file_index {
    std::string hash;
    FileMode mode;
};

void add_command(const std::vector<std::string>& args, std::string& initialized_repo);
void check_add_arguments(const std::vector<std::string>& args);

#endif
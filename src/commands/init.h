#ifndef INIT_H
#define INIT_H

#include <vector>
#include <string>

void check_if_already_initalised(const std::string& target_dir);
void create_codlog_directory(const std::string& target_dir);
void check_init_arguments(const std::vector<std::string>& args);
void init_command(const std::vector<std::string>& args);

#endif
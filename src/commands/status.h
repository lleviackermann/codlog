#ifndef STATUS_H
#define STATUS_H

#include <vector>
#include <string>
#include <filesystem>

bool files_modified(const std::filesystem::path& working_dir);
void check_arguments(const std::vector<std::string> &args);
void status_command(const std::vector<std::string> &args, std::string &initialized_repo);

#endif
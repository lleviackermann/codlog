#ifndef STATUS_H
#define STATUS_H

#include <vector>
#include <string>
#include <filesystem>

std::vector<std::filesystem::path> get_all_file_paths(const std::filesystem::path &initialized_repo);
bool files_modified(const std::filesystem::path &initialized_repo);
void check_arguments(const std::vector<std::string> &args);
void status_command(const std::vector<std::string> &args, std::string &initialized_repo);

#endif
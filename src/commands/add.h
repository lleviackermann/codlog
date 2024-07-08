#ifndef ADD_H
#define ADD_H

#include <filemode.h>
#include <helper.h>
#include <vector>
#include <string>
#include <queue>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <atomic>

void add_command(const std::vector<std::string> &args, std::string &initialized_repo);
void check_add_arguments(const std::vector<std::string> &args);
void get_all_file_paths(const std::vector<std::string> &args, std::queue<std::filesystem::path> &filePathStore, std::string &initialized_repo);
void process_files(std::queue<std::filesystem::path>& filePathStore,
                   const std::filesystem::path& repo_path,
                   std::atomic<bool>& done);
void create_blobs_and_get_hashes(std::queue<std::filesystem::path> &filePathStore, const std::filesystem::path &repo_path);


#endif
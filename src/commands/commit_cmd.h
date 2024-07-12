#ifndef COMMIT_CMD_H
#define COMMIT_CMD_H

#include <helper.h>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <string>

int make_tree_and_store(std::unordered_map<std::filesystem::path, TreeEntry> &obj_info, const std::filesystem::path &cur_dir, const std::filesystem::path &repo_path);
void validate_args(const std::vector<std::string> &args);
std::pair<std::string, std::string> get_committer_details(const std::filesystem::path &initialized_repo);
std::string get_ref_head_path(std::string &initialized_repo);
std::string get_current_commit_hash(std::string &initialized_repo);
void upateRefFile(std::string &initialized_repo, std::string& currentHash);
void commit_command(const std::vector<std::string> &args, std::string &initialized_repo);

#endif


#include <commit_cmd.h>
#include <config.h>
#include <stage.h>
#include <helper.h>
#include <tree.h>
#include <commit_obj.h>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace
{
    std::string commit_message = "";
    std::unique_ptr<StagingArea> previousStaged;
    std::unique_ptr<StagingArea> currentStaged;
    std::unordered_set<std::string> previous_committed_files;
    std::unordered_set<std::string> current_staged_files;
    std::unordered_map<std::string, index_entry> previous_files_entries;
    std::unordered_map<std::string, index_entry> current_files_entries;
    std::string blob = "blob";
    std::string tree__ = "tree";
    std::string tree_mode = "040000";
}

int make_tree_and_store(std::unordered_map<std::filesystem::path, TreeEntry> &obj_info, const std::filesystem::path &cur_dir, const std::filesystem::path &repo_path)
{
    // std::cout << cur_dir << "\n";
    // return 0;
    if (cur_dir.filename() == ".codlog" || cur_dir.filename() == ".git")
        return 0;
    int file_no = 0;
    Tree cur_tree;
    for (const auto &cur_path : std::filesystem::directory_iterator(cur_dir))
    {
        const std::filesystem::path cur_path_relative = std::filesystem::relative(cur_path, repo_path);
        if (std::filesystem::is_directory(cur_path))
        {
            // std::cout << cur_dir << " " << cur_path << "\n";
            int cnt = make_tree_and_store(std::ref(obj_info), cur_path, repo_path);
            file_no += cnt;
            if (cnt != 0)
            {
                cur_tree.addEntry(obj_info[cur_path_relative]);
            }
        }
        else
        {
            if (current_staged_files.count(cur_path_relative.string()))
            {
                const index_entry details = current_files_entries[cur_path_relative.string()];
                TreeEntry temporary = TreeEntry(details.obj_mode, blob, details.obj_hash, cur_path_relative.filename().string());
                cur_tree.addEntry(temporary);
                file_no++;
            }
            else if (previous_committed_files.count(cur_path_relative.string()))
            {
                const index_entry details = previous_files_entries[cur_path_relative.string()];
                TreeEntry temporary = TreeEntry(details.obj_mode, blob, details.obj_hash, cur_path_relative.filename().string());
                cur_tree.addEntry(temporary);
                file_no++;
            }
        }
    }
    if (file_no)
    {
        std::filesystem::path obj_directory = repo_path / ".codlog" / "objects";
        TreeEntry cur_tree_entry(tree_mode, tree__, cur_tree.calculateHash(), cur_dir.filename());
        std::filesystem::path cur_dir_relative = std::filesystem::relative(cur_dir, repo_path);
        cur_tree.writeToFile(obj_directory);
        if (cur_dir == repo_path)
            cur_dir_relative = "./";
        obj_info[cur_dir_relative] = std::move(cur_tree_entry);
    }
    return file_no;
}

void validate_args(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::invalid_argument("Invalid number of arguments. Usage: codlog commit [-m \"commit message\"]");
    }

    size_t start_index = 0;

    if (args.size() > start_index && args[start_index] == "-m")
    {
        if (args.size() <= start_index + 1)
        {
            throw std::invalid_argument("Missing commit message after '-m'.");
        }

        start_index += 1;
        for (; start_index < args.size() - 1; start_index++)
            commit_message += args[start_index];
    }
}

std::pair<std::string, std::string> get_committer_details(const std::filesystem::path &initialized_repo)
{
    std::string username, email;

    // Check local config
    username = Config::get_local_username(initialized_repo);
    email = Config::get_local_email(initialized_repo);

    if (username.empty() || email.empty())
    {
        username = Config::get_global_username();
        email = Config::get_global_email();
    }

    if (username.empty() || email.empty())
    {
        std::ostringstream error_msg;
        error_msg << "Committer details not set. Please set your username and email using the following commands:\n"
                  << "codlog config --set <global|local>.username \"Your Name\"\n"
                  << "codlog config --set <global|local>.email \"your.email@example.com\"\n"
                  << "Replace <global|local> with 'global' for system-wide settings or 'local' for repository-specific settings.";
        throw std::invalid_argument(error_msg.str());
    }

    return std::make_pair(username, email);
}

std::string get_ref_head_path(std::string &initialized_repo)
{
    const std::filesystem::path heads_file_path = initialized_repo + "/.codlog/HEAD";
    if (!std::filesystem::exists(heads_file_path))
    {
        throw std::runtime_error("Heads file does not exist " + heads_file_path.string());
    }
    std::ifstream heads_file(heads_file_path);

    if (!heads_file.is_open())
    {
        throw std::runtime_error("Unable to open Head file: " + heads_file_path.string());
    }
    std::string line;
    if (std::getline(heads_file, line))
    {
    std::string temp, main_ref_path;
        std::istringstream iss(line);
        iss >> temp >> main_ref_path;
        std::cout << temp << " main ref path " << main_ref_path << std::endl;
        if (heads_file.is_open())
            heads_file.close();
        return initialized_repo + "/.codlog/" + main_ref_path;
    }
    else
    {
        if (heads_file.is_open())
            heads_file.close();
        throw std::runtime_error("Head file Empty file: " + heads_file_path.string());
    }
}

std::string get_previous_commit_hash(std::string &initialized_repo)
{

    std::filesystem::path main_ref_path = get_ref_head_path(initialized_repo);

    if (!std::filesystem::exists(main_ref_path))
    {
        throw std::runtime_error("Main ref Heads file does not exist " + main_ref_path.string());
    }

    std::ifstream main_file(main_ref_path);
    if (!main_file.is_open())
    {
        throw std::runtime_error("Main ref heads Unable to open file: " + main_ref_path.string());
    }
    
    std::string parentHash;
    if (std::getline(main_file, parentHash))
    {
        if (main_file.is_open())
            main_file.close();
        return parentHash;
    }
    else
    {
        if (main_file.is_open())
            main_file.close();
        throw std::runtime_error("Main ref file Empty file: " + main_ref_path.string());
    }
}

void updateRefFile(std::string &initialized_repo, std::string& currentHash)
{
    std::filesystem::path main_ref_path = get_ref_head_path(initialized_repo);

    std::ofstream main_file(main_ref_path);
    if (!main_file.is_open())
    {
        throw std::runtime_error("Main ref heads Unable to open file: " + main_ref_path.string());
    }
    main_file << currentHash;
    main_file.close();
}


void commit_command(const std::vector<std::string> &args, std::string &initialized_repo)
{
    // check_add_arguments(args);
    validate_args(args);
    auto [committerName, committerEmail] = get_committer_details(std::filesystem::path(initialized_repo));
    const std::filesystem::path index_file_path = initialized_repo + "/.codlog/index";
    const std::filesystem::path staged_file_path = initialized_repo + "/.codlog/staged";
    previousStaged = std::make_unique<StagingArea>(index_file_path);
    currentStaged = std::make_unique<StagingArea>(staged_file_path);
    previous_files_entries = previousStaged->get_entries();
    current_files_entries = currentStaged->get_entries();
    for (auto [filePath, _] : previous_files_entries)
        previous_committed_files.insert(filePath);
    for (auto [filePath, _] : current_files_entries)
        current_staged_files.insert(filePath);
    std::unordered_map<std::filesystem::path, TreeEntry> obj_info;
    make_tree_and_store(obj_info, std::filesystem::path(initialized_repo), std::filesystem::path(initialized_repo));
    for (auto [fir, sec] : obj_info)
    {
        std::cout << fir << "->\n";
        std::cout << sec;
    }
    std::string previous_commit_hash = get_previous_commit_hash(initialized_repo);
    Commit current_commit(commit_message, obj_info["./"].obj_hash, previous_commit_hash, committerName, committerEmail);
    const std::string objects_dir = initialized_repo + "/.codlog/objects";
    current_commit.writeToFile(objects_dir);
    std::string current_commit_hash = current_commit.getHash();
    updateRefFile(initialized_repo, current_commit_hash);
}
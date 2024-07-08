#include <stage.h>
#include <status.h>
#include <blob.h>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <filesystem>
#include <thread>
#include <mutex>
#include <future>

std::vector<std::filesystem::path> get_all_file_paths(const std::filesystem::path &initialized_repo)
{
    const std::filesystem::path repo_path = initialized_repo;

    std::queue<std::filesystem::path> directory_store;
    directory_store.push(repo_path);
    std::vector<std::filesystem::path> filePathStore;
    while (directory_store.size())
    {
        std::filesystem::path first_dir = directory_store.front();
        directory_store.pop();
        for (auto const &dir_entry : std::filesystem::directory_iterator(first_dir))
        {
            if (dir_entry.path().filename() == ".codlog" || dir_entry.path().filename() == ".git")
                continue;
            if (!std::filesystem::is_directory(dir_entry))
            {
                std::filesystem::path relative_path = std::filesystem::relative(dir_entry, repo_path);
                filePathStore.push_back(relative_path);
            }
            else
            {
                directory_store.push(dir_entry);
            }
        }
    }
    return filePathStore;
}

bool files_modified(const std::filesystem::path &initialized_repo)
{
    const std::filesystem::path index_file_path = initialized_repo / ".codlog/index";
    const std::filesystem::path staged_file_path = initialized_repo / ".codlog/staged";

    StagingArea indexing_area(index_file_path);
    StagingArea staging_area(staged_file_path);
    std::vector<std::filesystem::path> all_files_current = get_all_file_paths(initialized_repo);

    std::vector<std::string> staged_modified;
    std::vector<std::string> unstaged_modified;
    std::vector<std::string> staged_untracked;
    std::vector<std::string> unstaged_untracked;
    std::vector<std::string> deleted_files;

    std::unordered_set<std::string> index_files;
    std::unordered_set<std::string> staged_files;

    std::unordered_set<std::string> current_files;

    std::unordered_map<std::string, index_entry> index_entries = indexing_area.get_entries();
    std::unordered_map<std::string, index_entry> staged_entries = staging_area.get_entries();

    for (const auto &entry : index_entries)
    {
        index_files.insert(entry.first);
    }

    for (const auto &entry : staged_entries)
    {
        staged_files.insert(entry.first);
    }

    std::mutex staged_modified_mutex, unstaged_modified_mutex, staged_untracked_mutex, unstaged_untracked_mutex, deleted_mutex;

    auto process_files = [&](std::vector<std::filesystem::path>::iterator begin,
                             std::vector<std::filesystem::path>::iterator end)
    {
        for (auto it = begin; it != end; ++it)
        {
            std::string file_path = it->string();
            current_files.insert(file_path);
            const std::filesystem::path fi_path = initialized_repo / file_path;
            Blob blob(fi_path);
            std::string cur_file_hash = blob.getHash();
            if (staged_files.count(file_path))
            {
                if (index_files.count(file_path))
                {
                    std::lock_guard<std::mutex> lock(staged_modified_mutex);
                    staged_modified.push_back(file_path);
                    if (cur_file_hash != staged_entries[file_path].obj_hash)
                    {
                        std::lock_guard<std::mutex> lock_under(unstaged_modified_mutex);
                        unstaged_modified.push_back(file_path);
                    }
                }
                else
                {
                    std::lock_guard<std::mutex> lock(staged_untracked_mutex);
                    staged_untracked.push_back(file_path);
                    if (cur_file_hash != staged_entries[file_path].obj_hash)
                    {
                        std::lock_guard<std::mutex> lock_under(unstaged_modified_mutex);
                        unstaged_modified.push_back(file_path);
                    }
                }
            }
            else
            {
                if (index_files.count(file_path))
                {
                    if (cur_file_hash != index_entries[file_path].obj_hash)
                    {
                        std::lock_guard<std::mutex> lock_under(unstaged_modified_mutex);
                        unstaged_modified.push_back(file_path);
                    }
                }
                else
                {
                    std::lock_guard<std::mutex> lock(unstaged_untracked_mutex);
                    unstaged_untracked.push_back(file_path);
                }
            }
        }
    };

    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::future<void>> futures;

    if (!all_files_current.empty())
    {
        size_t files_per_thread = (all_files_current.size() + num_threads - 1) / num_threads;
        auto chunk_begin = all_files_current.begin();

        while (chunk_begin != all_files_current.end())
        {
            auto chunk_end = std::min(chunk_begin + files_per_thread, all_files_current.end());
            futures.push_back(std::async(std::launch::async, process_files, chunk_begin, chunk_end));
            chunk_begin = chunk_end;
        }

        for (auto &future : futures)
        {
            future.wait();
        }
    }

    // Check for deleted files
    for (const auto &index_file : index_files)
    {
        if (current_files.count(index_file) == 0)
        {
            std::lock_guard<std::mutex> lock(deleted_mutex);
            deleted_files.push_back(index_file);
        }
    }

    // Sort the vectors for consistent output
    std::sort(staged_modified.begin(), staged_modified.end());
    std::sort(unstaged_modified.begin(), unstaged_modified.end());
    std::sort(staged_untracked.begin(), staged_untracked.end());
    std::sort(unstaged_untracked.begin(), unstaged_untracked.end());
    std::sort(deleted_files.begin(), deleted_files.end());

    // Print status in Git-like format
    if(staged_modified.size() || staged_untracked.size() || deleted_files.size()) {
        std::cout << "Changes to be committed:\n";
        for (const auto &file : staged_modified)
        {
            std::cout << "  (modified): " << file << "\n";
        }
        for (const auto &file : staged_untracked)
        {
            std::cout << "  (new file): " << file << "\n";
        }
        for (const auto &file : deleted_files)
        {
            std::cout << "  (deleted file): " << file << "\n";
        }
    }

    if(unstaged_modified.size() || unstaged_untracked.size()) {
        std::cout << "\nChanges not staged for commit:\n";
        for (const auto &file : unstaged_modified)
        {
            std::cout << "  (modified): " << file << "\n";
        }
        std::cout << "\n  Untracked files:\n";
        for (const auto &file : unstaged_untracked)
        {
            std::cout << "    " << file << "\n";
        }
    }

    return !staged_modified.empty() || !staged_untracked.empty() || !unstaged_modified.empty() || !unstaged_untracked.empty() || !deleted_files.empty();
    // return true;
}

void check_arguments(const std::vector<std::string> &args)
{
    if (args.size() > 1)
    {
        throw std::runtime_error("Error: 'status' command takes no arguments");
    }
}

void status_command(const std::vector<std::string> &args, std::string &initialized_repo)
{
    check_arguments(args);
    bool flag__ = files_modified(std::filesystem::path(initialized_repo));

    if (!flag__) {
        std::cout << "Nothing to commit, working tree clean\n";
    }
}
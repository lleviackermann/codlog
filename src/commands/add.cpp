#include <add.h>
#include <blob.h>
#include <helper.h>
#include <stage.h>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <filesystem>
#include <thread>
#include <mutex>

namespace {
    std::mutex queue_mutex;
    std::mutex index_file_mutex;
    std::mutex staged_file_mutex;
    std::string blob_def = "blob";
    std::mutex file_write_mutex;
    std::mutex cout_lock;
    std::unique_ptr<StagingArea> index_file_entries;
    std::unique_ptr<StagingArea> staged_file_entries;
}

void check_add_arguments(const std::vector<std::string>& args) {
    if (args.empty()) {
        throw std::invalid_argument("No arguments provided.");
    }

    std::filesystem::path directory = args.back();
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        throw std::runtime_error("File_path: ./src/commands/add.cpp\nFunction: check_add_arguments\nError: The specified directory does not exist or is not a directory: " + directory.string());
    }

    if (args.size() == 1) {
        throw std::invalid_argument("No files specified to add.");
    }

    if (args.size() == 2 && args[0] == ".") {
        // Special case: adding all files (.)
        return;
    }

    // Check each file argument (excluding the directory)
    for (size_t i = 0; i < args.size() - 1; ++i) {
        if (args[i] == ".") {
            throw std::invalid_argument("'.' must be the only file argument if specified.");
        }
        std::filesystem::path file_path = directory / args[i];
        if (!std::filesystem::exists(file_path)) {
            throw std::invalid_argument("File does not exist: " + file_path.string());
        }
        if (std::filesystem::is_directory(file_path)) {
            throw std::invalid_argument("Cannot add a directory: " + file_path.string());
        }
    }
}

void get_all_file_paths(const std::vector<std::string>& args, std::queue<std::filesystem::path>& filePathStore, std::string& initialized_repo) {
    const std::filesystem::path cur_dir = args.back();
    const std::filesystem::path repo_path = std::filesystem::path(initialized_repo);

    if(args[0] == ".") {
        std::queue<std::filesystem::path> directory_store;
        directory_store.push(cur_dir);
        while(directory_store.size()) {
            std::filesystem::path first_dir = directory_store.front();
            directory_store.pop();
            for(auto const& dir_entry : std::filesystem::directory_iterator(first_dir)) {
                if(dir_entry.path().filename() == ".codlog" || dir_entry.path().filename() == ".git") continue;
                if(!std::filesystem::is_directory(dir_entry)) {
                    std::filesystem::path relative_path = std::filesystem::relative(dir_entry, repo_path);
                    filePathStore.push(relative_path);
                } else {
                    directory_store.push(dir_entry);
                }
            }
        
        }
    } else {
        for(size_t num = 0; num < args.size() - 1; num++) {
            const std::filesystem::path file_path = cur_dir / args[num];
            std::filesystem::path relative_path = std::filesystem::relative(file_path, repo_path);
            filePathStore.push(relative_path);
        }
    }
}

void process_files(std::queue<std::filesystem::path>& filePathStore,
                   const std::filesystem::path& repo_path,
                   std::atomic<bool>& done) {
    while (true) {
        std::filesystem::path relative_path;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            // std::cout << filePathStore.size() << " st " << filePathStore.empty() << "  done " << done << "  ";
            if (filePathStore.empty()) {
                // if(done) return;
                // continue;
                return;
            }
            relative_path = filePathStore.front();
            filePathStore.pop();
        }

        std::filesystem::path full_path = repo_path / relative_path;
        std::filesystem::path obj_directory = repo_path / ".codlog" / "objects";
        {
            std::lock_guard<std::mutex> coulo(cout_lock);
            std::cout << full_path << " " << std::filesystem::exists(full_path) << std::endl;
        }
        try {
            Blob blob(full_path);
            std::string file_hash = blob.getHash();
            {
                std::lock_guard<std::mutex> locki(file_write_mutex);
                blob.writeToFile(obj_directory);
            }
            bool flag__ = false;
            {
                std::lock_guard<std::mutex> index_lock(index_file_mutex);
                std::pair<std::string, std::string> ent = index_file_entries->get_index_entry(relative_path);
                if(ent.second == file_hash) flag__ = true;
            }
            {
                std::lock_guard<std::mutex> lock(staged_file_mutex);
                if(flag__) staged_file_entries->delete_entry(relative_path);
                else staged_file_entries->update_or_add_entry(relative_path, blob.getFileMode(), file_hash);
            }
            
            // return;  // We've processed a file, so our job is done
        } catch (const std::exception& e) {
            throw std::runtime_error("File: src/commands/add\nFunction process_files\nError: Error creating blob for file: " + full_path.string() + "\n" + e.what());
        }
    }
}

void create_blobs_and_get_hashes(std::queue<std::filesystem::path>& filePathStore, const std::filesystem::path& repo_path) {
    
    std::atomic<bool> done(false);

    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(
            static_cast<void(*)(std::queue<std::filesystem::path>&,
                                const std::filesystem::path&,
                                std::atomic<bool>&)>(process_files),
            std::ref(filePathStore),
            std::ref(repo_path),
            std::ref(done)
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }
    done = true;
}

void add_command(const std::vector<std::string>& args, std::string& initialized_repo) {
    check_add_arguments(args);
    std::queue<std::filesystem::path> filePathStore;
    get_all_file_paths(args, filePathStore, initialized_repo);
    const std::filesystem::path index_file_path = initialized_repo + "/.codlog/index";
    const std::filesystem::path staged_file_path = initialized_repo + "/.codlog/staged";
    index_file_entries = std::make_unique<StagingArea>(index_file_path);
    staged_file_entries = std::make_unique<StagingArea>(staged_file_path);
    create_blobs_and_get_hashes(filePathStore, initialized_repo);
    staged_file_entries->write_to_index_file(staged_file_path);
}
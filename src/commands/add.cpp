#include <add.h>
#include <blob.h>
#include <helper.h>
#include <tree.h>
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
    std::mutex map_mutex;
    std::string blob_def = "blob";
    std::string tree_def = "tree";
    std::string tree_mode = "040000";
    std::mutex file_write_mutex;
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
                   std::unordered_map<std::filesystem::path, TreeEntry>& obj_info,
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
        try {
            Blob blob(full_path);
            {
                std::lock_guard<std::mutex> locki(file_write_mutex);
                blob.writeToFile(obj_directory);
            }
            TreeEntry temp_blob(blob.getFileMode(), blob_def, blob.getHash(), full_path.filename().string());
            {
                std::lock_guard<std::mutex> lock(map_mutex);
                obj_info[relative_path] = std::move(temp_blob);
            }
            
            // return;  // We've processed a file, so our job is done
        } catch (const std::exception& e) {
            throw std::runtime_error("File: src/commands/add\nFunction process_files\nError: Error creating blob for file: " + full_path.string() + "\n" + e.what());
        }
    }
}

void create_blobs_and_get_hashes(std::queue<std::filesystem::path>& filePathStore, const std::filesystem::path& repo_path, std::unordered_map<std::filesystem::path, TreeEntry>& obj_info) {
    
    std::atomic<bool> done(false);

    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(
            static_cast<void(*)(std::queue<std::filesystem::path>&,
                                std::unordered_map<std::filesystem::path, TreeEntry>&,
                                const std::filesystem::path&,
                                std::atomic<bool>&)>(process_files),
            std::ref(filePathStore),
            std::ref(obj_info),
            std::ref(repo_path),
            std::ref(done)
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }
    done = true;
}

int make_tree_and_store(std::unordered_map<std::filesystem::path, TreeEntry>& obj_info, const std::filesystem::path& cur_dir, const std::filesystem::path& repo_path) {
    // std::cout << cur_dir << "\n";
    // return 0;
    if(cur_dir.filename() == ".codlog" || cur_dir.filename() == ".git") return 0;
    int file_no = 0;
    Tree cur_tree;
    for(const auto& cur_path : std::filesystem::directory_iterator(cur_dir)) {
        const std::filesystem::path cur_path_relative = std::filesystem::relative(cur_path, repo_path);
        if(std::filesystem::is_directory(cur_path)) {
            // std::cout << cur_dir << " " << cur_path << "\n";
            int cnt = make_tree_and_store(std::ref(obj_info), cur_path, repo_path);
            file_no += cnt;
            if(cnt != 0) {
                cur_tree.addEntry(obj_info[cur_path_relative]);
            }
        } else {
            cur_tree.addEntry(obj_info[cur_path_relative]);
            file_no++;
        }
    }
    if(file_no) {
        std::filesystem::path obj_directory = repo_path / ".codlog" / "objects";
        TreeEntry cur_tree_entry(tree_mode, tree_def, cur_tree.calculateHash(), cur_dir.filename());
        std::filesystem::path cur_dir_relative = std::filesystem::relative(cur_dir, repo_path);
        cur_tree.writeToFile(obj_directory);
        if(cur_dir == repo_path) cur_dir_relative = "./";
        obj_info[cur_dir_relative] = std::move(cur_tree_entry);
    }
    return file_no;
}

void add_command(const std::vector<std::string>& args, std::string& initialized_repo) {
    check_add_arguments(args);
    std::queue<std::filesystem::path> filePathStore;
    get_all_file_paths(args, filePathStore, initialized_repo);
    std::unordered_map<std::filesystem::path, TreeEntry> obj_info;
    create_blobs_and_get_hashes(filePathStore, initialized_repo, obj_info);
    make_tree_and_store(obj_info, initialized_repo, initialized_repo);
    for(auto [pat, has] : obj_info) {
        if(has.obj_type == "blob") continue;
        std::cout << pat << " -> " << has << std::endl;
    }
    // std::filesystem::path directory = args.back();
    // std::filesystem::path file_path = directory / args[0];
    // Blob temp(file_path);
    // std::string hash = temp.getHash();
    // std::cout << hash << std::endl;
    // std::cout << temp.getFileMode() << "\n";
    // for(auto ch : temp.getContent()) std::cout << ch;
    std::cout << "initalised repo " << initialized_repo << "\n";
}
#include <add.h>
#include <blob.h>
#include <helper.h>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <filesystem>

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

void add_command(const std::vector<std::string>& args, std::string& initialized_repo) {
    check_add_arguments(args);
    std::unordered_map<std::filesystem::path, file_index> blobs_store;
    std::filesystem::path directory = args.back();
    std::filesystem::path file_path = directory / args[0];
    Blob temp(file_path);
    std::string hash = temp.getHash();
    std::cout << hash << std::endl;
    std::cout << temp.getFileMode() << "\n";
    std::cout << "initalised repo " << initialized_repo << "\n";
}
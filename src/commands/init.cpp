#include <init.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

void check_if_already_initalised(const std::string& target_dir) {
    std::filesystem::path target_dir_path = target_dir;
    std::filesystem::path codlog_dir = target_dir_path / ".codlog";

    if (std::filesystem::exists(codlog_dir)) {
        throw std::invalid_argument("Codlog repository already initialized in " + codlog_dir.string() + ".");
    }
}

void create_codlog_directory(const std::string& target_dir) {
    std::filesystem::path target_dir_path = target_dir;
    if(!std::filesystem::exists(target_dir)) {
        std::filesystem::create_directories(target_dir_path);
    }
    std::filesystem::path codlog_dir = target_dir_path / ".codlog";

    std::filesystem::create_directory(codlog_dir);
    std::filesystem::create_directory(codlog_dir / "objects");
    std::filesystem::create_directory(codlog_dir / "refs");
    std::filesystem::create_directory(codlog_dir / "refs" / "heads");
    std::ofstream index_file(codlog_dir / "index");
    if(index_file.is_open()) {
        index_file.close();
    }

    std::ofstream head_file(codlog_dir / "HEAD");
    if (!head_file.is_open()) {
        throw std::runtime_error("File_path: ./src/commands/init.cpp\nFunction: create_codlog_directory\nError: Unable to create HEAD file");
    }
    head_file << "ref: refs/heads/main\n";
    head_file.close();
    std::cout << "Initialized empty Codlog repository in " << codlog_dir << std::endl;
}

void check_init_arguments(const std::vector<std::string>& args) {
    if (args.size() > 2) {
        throw std::invalid_argument("Too many arguments. Usage: codlog init [directory_name]");
    }
}


void init_command(const std::vector<std::string>& args) {
    check_init_arguments(args);
    const std::string target_dir = args.back() + (args.size() > 1 ? "/" + args[0] : "");
    check_if_already_initalised(target_dir);
    create_codlog_directory(target_dir);
}
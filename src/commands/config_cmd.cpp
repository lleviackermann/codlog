#include <config_cmd.h>
#include <config.h>
#include <iostream>
#include <string>
#include <filesystem>

std::filesystem::path find_codlog_root(const std::filesystem::path& start_path) {
    std::filesystem::path current_path = std::filesystem::absolute(start_path);
    
    while (true) {
        if (std::filesystem::exists(current_path / ".codlog")) {
            return current_path;
        }
        
        if (current_path == current_path.root_path()) {
            // We've reached the root directory without finding .codlog
            return {};
        }
        
        current_path = current_path.parent_path();
    }
}

void config_command(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        std::cerr << "Usage: codlog config --set <global|local>.<username|email> \"<value>\"" << std::endl;
        return;
    }

    std::string set_flag = args[0];
    std::string config_string = args[1];
    std::string value = args[2];
    std::filesystem::path current_path = args[3];  // The last argument is always the current path

    if (set_flag != "--set") {
        std::cerr << "Invalid flag. Use '--set'" << std::endl;
        return;
    }

    size_t dot_pos = config_string.find('.');
    if (dot_pos == std::string::npos) {
        std::cerr << "Invalid config format. Use <global|local>.<username|email>" << std::endl;
        return;
    }

    std::string scope = config_string.substr(0, dot_pos);
    std::string key = config_string.substr(dot_pos + 1);

    if (scope != "global" && scope != "local") {
        std::cerr << "Invalid scope. Use 'global' or 'local'" << std::endl;
        return;
    }

    if (key != "username" && key != "email") {
        std::cerr << "Invalid key. Use 'username' or 'email'" << std::endl;
        return;
    }

    if (value.empty()) {
        std::cerr << "Value cannot be empty" << std::endl;
        return;
    }

    if (scope == "global") {
        if (key == "username") {
            Config::set_global_username(value);
            std::cout << "Global username set to: " << value << std::endl;
        } else {
            Config::set_global_email(value);
            std::cout << "Global email set to: " << value << std::endl;
        }
    } else {
        std::filesystem::path repo_root = find_codlog_root(current_path);
        if (repo_root.empty()) {
            std::cerr << "Error: Not a codlog repository.\n"
                      << "Neither the current directory nor any parent directory is initialized as a codlog repository.\n"
                      << "To set local configurations, please run this command from within a codlog-initialized repository.\n"
                      << "You can initialize a new repository using 'codlog init'." << std::endl;
            return;
        }
        if (key == "username") {
            Config::set_local_username(repo_root, value);
            std::cout << "Local username set to: " << value << std::endl;
        } else {
            Config::set_local_email(repo_root, value);
            std::cout << "Local email set to: " << value << std::endl;
        }
    }
}
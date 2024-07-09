#include <config_cmd.h>
#include <config.h>
#include <iostream>
#include <string>
#include <filesystem>

void config_command(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        std::cerr << "Usage: codlog config <global|local>.<username|email>=<value>" << std::endl;
        return;
    }

    std::string config_string = args[0];
    std::filesystem::path repo_path = args[1];  // The last argument is always the repo path

    size_t dot_pos = config_string.find('.');
    size_t equals_pos = config_string.find('=');

    if (dot_pos == std::string::npos || equals_pos == std::string::npos || dot_pos > equals_pos) {
        std::cerr << "Invalid config format. Use <global|local>.<username|email>=<value>" << std::endl;
        return;
    }

    std::string scope = config_string.substr(0, dot_pos);
    std::string key = config_string.substr(dot_pos + 1, equals_pos - dot_pos - 1);
    std::string value = config_string.substr(equals_pos + 1);

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
        if (!std::filesystem::exists(repo_path / ".codlog")) {
            std::cerr << "Not a codlog repository (or any of the parent directories)" << std::endl;
            return;
        }
        if (key == "username") {
            Config::set_local_username(repo_path, value);
            std::cout << "Local username set to: " << value << std::endl;
        } else {
            Config::set_local_email(repo_path, value);
            std::cout << "Local email set to: " << value << std::endl;
        }
    }
}
#include <config.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>

void Config::init_config(const std::filesystem::path& repo_path) {
    std::filesystem::path global_config_path = get_global_config_path();
    std::filesystem::path local_config_path = get_local_config_path(repo_path);

    // Create global config if it doesn't exist
    if (!std::filesystem::exists(global_config_path)) {
        std::filesystem::create_directories(global_config_path.parent_path());
        std::ofstream global_file(global_config_path);
        if (global_file.is_open()) {
            global_file << "global.username=\n";
            global_file << "global.email=\n";
            global_file.close();
        }
    }

    // Create local config if it doesn't exist
    if (!std::filesystem::exists(local_config_path)) {
        std::filesystem::create_directories(local_config_path.parent_path());
        std::ofstream local_file(local_config_path);
        if (local_file.is_open()) {
            local_file << "local.username=\n";
            local_file << "local.email=\n";
            local_file.close();
        }
    }
}

std::string Config::get_global_username() {
    return get_value(get_global_config_path(), "global.username");
}

std::string Config::get_global_email() {
    return get_value(get_global_config_path(), "global.email");
}

std::string Config::get_local_username(const std::filesystem::path& repo_path) {
    return get_value(get_local_config_path(repo_path), "local.username");
}

std::string Config::get_local_email(const std::filesystem::path& repo_path) {
    return get_value(get_local_config_path(repo_path), "local.email");
}

void Config::set_global_username(const std::string& username) {
    set_value(get_global_config_path(), "global.username", username);
}

void Config::set_global_email(const std::string& email) {
    set_value(get_global_config_path(), "global.email", email);
}

void Config::set_local_username(const std::filesystem::path& repo_path, const std::string& username) {
    set_value(get_local_config_path(repo_path), "local.username", username);
}

void Config::set_local_email(const std::filesystem::path& repo_path, const std::string& email) {
    set_value(get_local_config_path(repo_path), "local.email", email);
}

std::filesystem::path Config::get_global_config_path() {
    std::filesystem::path config_path;

#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata != nullptr) {
        config_path = std::filesystem::path(appdata) / "codlog" / "config";
    } else {
        config_path = std::filesystem::path(std::getenv("USERPROFILE")) / "codlog" / "config";
    }
#else
    const char* home = std::getenv("HOME");
    if (home != nullptr) {
        config_path = std::filesystem::path(home) / ".config" / "codlog";
    } else {
        config_path = std::filesystem::current_path() / ".codlog" / "config";
    }
#endif

    return config_path;
}

std::filesystem::path Config::get_local_config_path(const std::filesystem::path& repo_path) {
    return repo_path / ".codlog" / "config";
}

std::string Config::get_value(const std::filesystem::path& config_path, const std::string& key) {
    std::ifstream file(config_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string current_key = line.substr(0, pos);
                if (current_key == key) {
                    return line.substr(pos + 1);
                }
            }
        }
        file.close();
    }
    return "";
}

void Config::set_value(const std::filesystem::path& config_path, const std::string& key, const std::string& value) {
    std::vector<std::string> lines;
    std::ifstream infile(config_path);
    if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos && line.substr(0, pos) == key) {
                line = key + "=" + value;
            }
            lines.push_back(line);
        }
        infile.close();
    }

    std::ofstream outfile(config_path);
    if (outfile.is_open()) {
        for (const auto& line : lines) {
            outfile << line << std::endl;
        }
        outfile.close();
    }
}
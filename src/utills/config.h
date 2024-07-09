#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <filesystem>

class Config {
public:
    static void init_config(const std::filesystem::path& repo_path);

    static std::string get_global_username();
    static std::string get_global_email();
    static std::string get_local_username(const std::filesystem::path& repo_path);
    static std::string get_local_email(const std::filesystem::path& repo_path);

    static void set_global_username(const std::string& username);
    static void set_global_email(const std::string& email);
    static void set_local_username(const std::filesystem::path& repo_path, const std::string& username);
    static void set_local_email(const std::filesystem::path& repo_path, const std::string& email);

private:
    static std::filesystem::path get_global_config_path();
    static std::filesystem::path get_local_config_path(const std::filesystem::path& repo_path);
    static std::string get_value(const std::filesystem::path& config_path, const std::string& key);
    static void set_value(const std::filesystem::path& config_path, const std::string& key, const std::string& value);
};

#endif // CONFIG_H
#include <helper.h>
#include <string>
#include <filesystem>

std::string get_initialised_repo(const std::string& target_dir) {
    std::filesystem::path target_dir_path = target_dir;

    // Iterate up the directory hierarchy from the target directory to the root
    for (std::filesystem::path current_dir = target_dir_path; current_dir.has_relative_path(); current_dir = current_dir.parent_path()) {
        std::filesystem::path codlog_dir = current_dir / ".codlog";
        if (std::filesystem::exists(codlog_dir)) {
            return codlog_dir.string();
        }
    }

    return "";
}
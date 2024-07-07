#include <helper.h>
#include <filemode.h>
#include <string>
#include <ostream>
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

std::ostream& operator<<(std::ostream& os, FileMode mode) {
    switch (mode) {
        case FileMode::EXECUTABLE:
            return os << "EXECUTABLE (" << static_cast<int>(mode) << ")";
        case FileMode::REGULAR:
            return os << "REGULAR (" << static_cast<int>(mode) << ")";
        case FileMode::DIRECTORY:
            return os << "DIRECTORY (" << static_cast<int>(mode) << ")";
        default:
            return os << "UNKNOWN (" << static_cast<int>(mode) << ")";
    }
}
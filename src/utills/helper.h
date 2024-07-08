#ifndef HELPER_H
#define HELPER_H

#include <filemode.h>
#include <string>
#include <ostream>

struct TreeEntry {
    std::string obj_mode;
    std::string obj_type;
    std::string obj_hash;
    std::string obj_name;

    TreeEntry() = default;

    TreeEntry(const std::string& mode, const std::string& type, const std::string& hash, const std::string& name);

    friend std::ostream& operator<<(std::ostream& os, const TreeEntry& entry);
};


std::string get_initialised_repo(const std::string& target_dir);
std::ostream& operator<<(std::ostream& os, FileMode mode);
std::string getFileModeStr(FileMode mode);

#endif
#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <helper.h>
#include <filesystem>

class Tree {
private:
    std::vector<TreeEntry> entries;

public:
    void addEntry(const std::string& mode, const std::string& obj_ty, const std::string& hash, const std::string& name);
    void addEntry(TreeEntry toadd);
    std::vector<unsigned char> serialize() const;
    std::string calculateHash();
    std::string toString() const;
    void writeToFile(const std::filesystem::path& output_directory);
};

#endif
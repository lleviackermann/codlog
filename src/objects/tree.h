#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <helper.h>

class Tree {
private:
    std::vector<TreeEntry> entries;

public:
    void addEntry(const std::string& mode, const std::string& obj_ty, const std::string& hash, const std::string& name);
    std::vector<unsigned char> serialize() const;
    std::string calculateHash() const;
    std::string toString() const;
};

#endif
#ifndef COMMIT_OBJ_H
#define COMMIT_OBJ_H

#include <string>
#include <vector>
#include <ctime>

class Commit
{
private:
    std::string treeHash;
    std::string parentTreeHash;
    std::string message;
    std::string committerName;
    std::string committerEmail;
    std::time_t timestamp;
    std::string hash;

public:
    Commit(const std::string &message, const std::string &treeHash, const std::string &parentTreeHash, const std::string &committerName, const std::string &committerEmail);

    std::string getHash() const;
    std::string getMessage() const;
    std::time_t getTimestamp() const;
    std::string getParentTreeHash() const;
    std::string getTreeHash() const;

    // Other methods
    std::string generateHash() const;
    std::string toString() const;

    void writeToFile(const std::string& objectsDir) const;
};

#endif
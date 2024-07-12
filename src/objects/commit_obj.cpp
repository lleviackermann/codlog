#include <commit_obj.h>
#include <config.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
Commit::Commit(const std::string &message, const std::string &treeHash, const std::string &parentTreeHash, const std::string &committerName, const std::string &committerEmail)
    : treeHash(treeHash), parentTreeHash(parentTreeHash), message(message), committerName(committerName), committerEmail(committerEmail)
{
    timestamp = std::time(nullptr);
    std::cout << timestamp << std::endl;
    hash = generateHash();
}

std::string Commit::getHash() const
{
    return hash;
}

std::string Commit::getMessage() const
{
    return message;
}

std::time_t Commit::getTimestamp() const
{
    return timestamp;
}

std::string Commit::getParentTreeHash() const
{
    return parentTreeHash;
}

std::string Commit::getTreeHash() const
{
    return treeHash;
}

std::string time_t_to_unix_timestamp_string(std::time_t time) {
    return std::to_string(time);
}

std::string Commit::generateHash() const
{
    try {
        const std::string strTimestamp = time_t_to_unix_timestamp_string(timestamp);
        std::vector<unsigned char> data;
        data.insert(data.end(), treeHash.begin(), treeHash.end());
        data.insert(data.end(), parentTreeHash.begin(), parentTreeHash.end());
        data.insert(data.end(), strTimestamp.begin(), strTimestamp.end());
        data.insert(data.end(), message.begin(), message.end());
        data.insert(data.end(), committerName.begin(), committerName.end());
        data.insert(data.end(), committerEmail.begin(), committerEmail.end());

        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(data.data(), data.size(), hash);
        std::stringstream ss;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return ss.str();
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        throw std::runtime_error("In generate hash \n");
    }

}

std::string Commit::toString() const
{
    std::stringstream ss;
    ss << "tree " << treeHash << "\n"
       << "parent " << parentTreeHash << "\n"
       << "committer " << committerName << " <"
       << committerEmail << "> " << std::ctime(&timestamp) << "\n"
       << "\n"
       << message << "\n";

    return ss.str();
}

void Commit::writeToFile(const std::string& objectsDir) const {
    if (hash.length() < 3) {
        throw std::runtime_error("Invalid hash length");
    }

    std::string subDir = hash.substr(0, 2);
    std::string fileName = hash.substr(2);

    std::filesystem::path fullPath = std::filesystem::path(objectsDir) / subDir / fileName;

    // Create subdirectory if it doesn't exist
    std::filesystem::create_directories(fullPath.parent_path());

    std::ofstream file(fullPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to create file: " + fullPath.string());
    }

    std::string content = toString();
    file.write(content.c_str(), content.length());

    if (!file) {
        if(file.is_open()) file.close();
        throw std::runtime_error("Error writing to file: " + fullPath.string());
    }

    file.close();
}

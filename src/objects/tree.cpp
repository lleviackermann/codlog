#include <helper.h>
#include <tree.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

void Tree::addEntry(const std::string &mode, const std::string &obj_ty, const std::string &hash, const std::string &name)
{
    entries.emplace_back(mode, obj_ty, hash, name);
}

std::vector<unsigned char> Tree::serialize() const
{
    std::vector<unsigned char> content;
    for (const auto &entry : entries)
    {
        // Add mode and name
        content.insert(content.end(), entry.obj_mode.begin(), entry.obj_mode.end());
        content.push_back(' ');
        content.insert(content.end(), entry.obj_name.begin(), entry.obj_name.end());
        content.push_back('\0');

        // Add hash (convert from hex to binary)
        for (size_t i = 0; i < entry.obj_hash.length(); i += 2)
        {
            unsigned char byte = std::stoi(entry.obj_hash.substr(i, 2), nullptr, 16);
            content.push_back(byte);
        }
    }

    // Prepare the header
    std::string header = "tree " + std::to_string(content.size()) + '\0';

    // Combine header and content
    std::vector<unsigned char> result(header.begin(), header.end());
    result.insert(result.end(), content.begin(), content.end());

    return result;
}

std::string Tree::calculateHash() const
{
    std::vector<unsigned char> content = serialize();
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(content.data(), content.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string Tree::toString() const
{
    std::stringstream ss;
    for (const auto &entry : entries)
    {
        ss << entry.obj_mode << " " << entry.obj_hash << " " << entry.obj_name << "\n";
    }
    return ss.str();
}
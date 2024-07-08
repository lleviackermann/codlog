#include <helper.h>
#include <tree.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <openssl/sha.h>

void Tree::addEntry(const std::string &mode, const std::string &obj_ty, const std::string &hash, const std::string &name)
{
    entries.emplace_back(mode, obj_ty, hash, name);
}

void Tree::addEntry(TreeEntry toadd) {
    entries.push_back(toadd);
}
std::vector<unsigned char> Tree::serialize() const
{
    std::vector<unsigned char> content;
    for (const auto &entry : entries)
    {
        // Add mode and name
        int flag = (entry.obj_mode == "040000");
        content.insert(content.end(), entry.obj_mode.begin() + flag, entry.obj_mode.end());
        content.push_back(' ');
        content.insert(content.end(), entry.obj_name.begin(), entry.obj_name.end());
        content.push_back('\0');

        // Add hash (convert from hex to binary)
        for (unsigned int i = 0; i < entry.obj_hash.length(); i += 2) {
            std::string byteString = entry.obj_hash.substr(i, 2);
            unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), NULL, 16));
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

std::string Tree::calculateHash()
{
    std::sort(entries.begin(), entries.end(), [](const TreeEntry& a, const TreeEntry& b) {
        return a.obj_name < b.obj_name;
    });
    // entries = std::move(copy_entries);
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
        ss << entry.obj_mode << " " << entry.obj_type << " " << entry.obj_hash << "   " << entry.obj_name << "\n";
    }
    return ss.str();
}

void Tree::writeToFile(const std::filesystem::path &output_directory)
{
    // Ensure output_directory exists
    if (!std::filesystem::exists(output_directory))
    {
        std::filesystem::create_directories(output_directory);
    }

    // Formulate the output file path
    std::string hash = std::move(calculateHash());
    std::filesystem::path output_file_sudir_path = output_directory / hash.substr(0, 2);

    if(!std::filesystem::exists(output_file_sudir_path)) {
        std::filesystem::create_directories(output_file_sudir_path);
    }

    std::filesystem::path output_file_path = output_file_sudir_path / hash.substr(2);
    // Check if the file already exists
    if (std::filesystem::exists(output_file_path))
    {
        return;
    }

    std::ofstream output_file(output_file_path, std::ios::binary | std::ios::out);
    std::vector<unsigned char> binary_content;
    for (const auto &entry : entries)
    {
        // Add mode and name
        binary_content.insert(binary_content.end(), entry.obj_mode.begin(), entry.obj_mode.end());
        binary_content.push_back(' ');
        binary_content.insert(binary_content.end(), entry.obj_type.begin(), entry.obj_type.end());
        binary_content.push_back(' ');
        binary_content.insert(binary_content.end(), entry.obj_hash.begin(), entry.obj_hash.end());
        binary_content.push_back(' ');
        binary_content.push_back(' ');
        binary_content.push_back(' ');
        binary_content.insert(binary_content.end(), entry.obj_name.begin(), entry.obj_name.end());
        binary_content.push_back('\n');
        // Add hash (convert from hex to binary)
    }
    if (!output_file.is_open())
    {
        throw std::runtime_error("File: blob.cpp\nFunction: writeToFile\nError: Failed to open file for writing: " + output_file_path.string());
    }

    // Set the file permissions to read and write
    std::filesystem::permissions(output_file_path, std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);

    // Write binary content to the file
    output_file.write(reinterpret_cast<const char *>(binary_content.data()), binary_content.size());

    if (!output_file.good())
    {
        output_file.close();
        throw std::runtime_error("File: blob.cpp\nFunction: writeToFile\nError: Error occurred while writing to file: " + output_file_path.string());
    }

    output_file.close();
}
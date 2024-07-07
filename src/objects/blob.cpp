// #include "blob.h"
#include <blob.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <iostream>

std::string Blob::calculateSHA1(const std::vector<unsigned char> &data)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data.data(), data.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

void Blob::read_file_binary()
{
    // Get the file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a vector to hold the file contents
    std::vector<unsigned char> buffer(size);

    // Read the file into the vector
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        throw std::runtime_error("File_Path: ./src/objects/blob.cpp\nFunction: read_file_binary\nError: Unable to open file: ");
    }

    file.close();
    binary_content = std::move(buffer);
}

Blob::Blob(const std::filesystem::path &file_path) : file(file_path, std::ios::binary | std::ios::ate)
{
    if (!file.is_open())
    {
        throw std::runtime_error("File_Path: ./src/objects/blob.cpp\nFunction: Blob Constructor\nError: Unable to open file: " + file_path.string());
    }
    read_file_binary();
    hash = calculateSHA1(serialize());
}

Blob::Blob(const std::vector<unsigned char>& binary_content) : binary_content(binary_content) 
{
    hash = calculateSHA1(serialize());
}

Blob::~Blob()
{
    if (file.is_open())
    {
        file.close();
    }
}

const std::vector<unsigned char> &Blob::getContent() const
{
    return binary_content;
}

const std::string &Blob::getHash() const
{
    return hash;
}

size_t Blob::getSize() const
{
    return binary_content.size();
}

std::vector<unsigned char> Blob::serialize() const
{
    std::vector<unsigned char> result;
    std::string header = "blob " + std::to_string(binary_content.size()) + '\0';
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), binary_content.begin(), binary_content.end());
    return result;
}

Blob Blob::deserialize(const std::vector<unsigned char> &data)
{
    size_t nullPos = 0;
    while (nullPos < data.size() && data[nullPos] != '\0')
    {
        nullPos++;
    }
    if (nullPos == data.size())
    {
        throw std::runtime_error("File_Path: ./src/objects/blob.cpp\nFunction: deserialize\nError: Invalid blob format");
    }

    std::vector<unsigned char> content(data.begin() + nullPos + 1, data.end());
    return Blob(content);
}

// int main() {
//     std::cout << " hi " << std::endl;
//     return 0;
// }
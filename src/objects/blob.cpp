#include <filemode.h>
#include <blob.h>
#include <helper.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <iostream>
#include <fstream>

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
    std::filesystem::file_status status = std::filesystem::status(file_path);
    if ((status.permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none)
        file_mode = FileMode::EXECUTABLE;
    else
        file_mode = FileMode::REGULAR;
    if (!file.is_open())
    {
        throw std::runtime_error("File_Path: ./src/objects/blob.cpp\nFunction: Blob Constructor\nError: Unable to open file: " + file_path.string());
    }
    read_file_binary();
    hash = calculateSHA1(getBlobHashContent());
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

const std::string Blob::getHash() const
{
    return hash;
}

const std::string Blob::getFileMode() const
{
    return getFileModeStr(file_mode);
}

size_t Blob::getSize() const
{
    return binary_content.size();
}

std::vector<unsigned char> Blob::getBlobHashContent() const
{
    std::vector<unsigned char> result;
    std::string header = "blob " + std::to_string(binary_content.size()) + '\0';
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), binary_content.begin(), binary_content.end());
    return result;
}

std::vector<unsigned char> Blob::getOriginalContent(const std::vector<unsigned char> &data)
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
    return content;
}

void Blob::writeToFile(const std::filesystem::path &output_directory) const
{
    // Ensure output_directory exists
    if (!std::filesystem::exists(output_directory))
    {
        std::filesystem::create_directories(output_directory);
    }

    // Formulate the output file path
    std::filesystem::path output_file_sudir_path = output_directory / hash.substr(0, 2);

    if(!std::filesystem::exists(output_file_sudir_path)) {
        std::filesystem::create_directories(output_file_sudir_path);
    }

    std::filesystem::path output_file_path = output_file_sudir_path / hash.substr(2);
    // Check if the file already exists
    // std::cout << output_file_path << std::endl;
    if (std::filesystem::exists(output_file_path))
    {
        return;
    }

    std::ofstream output_file(output_file_path, std::ios::binary | std::ios::out);
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
#ifndef BLOB_H
#define BLOB_H

#include <filemode.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

class Blob {
private:
    std::vector<unsigned char> binary_content;
    std::string hash;
    std::ifstream file;
    FileMode file_mode;
    std::string calculateSHA1(const std::vector<unsigned char>& data);
    void read_file_binary();
    std::vector<unsigned char> getBlobHashContent() const;

public:
    explicit Blob(const std::filesystem::path& file_path);
    ~Blob();

    // Disable copy constructor and assignment operator
    Blob(const Blob&) = delete;
    Blob& operator=(const Blob&) = delete;

    const std::vector<unsigned char>& getContent() const;
    const std::string getHash() const;
    const std::string getFileMode() const;
    size_t getSize() const;

    static std::vector<unsigned char> getOriginalContent(const std::vector<unsigned char>& data);
    void writeToFile(const std::filesystem::path& output_directory) const;
};

#endif 
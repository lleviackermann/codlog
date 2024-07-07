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

public:
    explicit Blob(const std::filesystem::path& file_path);
    explicit Blob(const std::vector<unsigned char>& binary_content);
    ~Blob();

    // Disable copy constructor and assignment operator
    Blob(const Blob&) = delete;
    Blob& operator=(const Blob&) = delete;

    const std::vector<unsigned char>& getContent() const;
    const std::filesystem::path& getFilePath() const;
    const std::string& getHash() const;
    const FileMode getFileMode() const;
    size_t getSize() const;

    std::vector<unsigned char> serialize() const;
    static Blob deserialize(const std::vector<unsigned char>& data);
};

#endif 
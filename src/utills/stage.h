#ifndef STAGE_H
#define STAGE_H

#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>
struct index_entry
{
    std::string obj_mode;
    std::string obj_hash;

    index_entry(const std::string &obj_mode, const std::string &obj_hash);
};

class StagingArea
{
private:
    std::unordered_map<std::string, index_entry> entries;
    std::ifstream index_file_read;
    std::ofstream index_file_write;
public:
    StagingArea() = default;
    StagingArea(const std::filesystem::path& index_file_path);
    ~StagingArea();

    void update_or_add_entry(const std::filesystem::path &file_path, const std::string &obj_mode, const std::string &obj_hash);
    void delete_entry(std::filesystem::path &deleted_file_path);
    void write_to_index_file(const std::filesystem::path& index_file_path);
    std::string to_string() const;
};

#endif
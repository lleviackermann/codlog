#include <stage.h>
#include <iostream>
#include <sstream>

index_entry::index_entry(const std::string& obj_mode, const std::string& obj_hash)
    : obj_mode(obj_mode), obj_hash(obj_hash) {}

StagingArea::StagingArea(const std::filesystem::path& index_file_path) {
    // Open the index file in read/write mode

    index_file_read.open(index_file_path, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!index_file_read) {
        // If the file doesn't exist, create it
        index_file_read.open(index_file_path, std::ios::out | std::ios::binary);
        index_file_read.close();
        index_file_read.open(index_file_path, std::ios::in | std::ios::out | std::ios::binary);
    }

    // Read existing entries from the index file
    std::string line;
    while (std::getline(index_file_read, line)) {
        std::istringstream iss(line);
        std::string obj_mode, obj_hash, file_path;
        iss >> obj_mode >> obj_hash >> file_path;
        // entries[file_path] = index_entry(obj_mode, obj_hash);
        entries.emplace(file_path, index_entry(obj_mode, obj_hash));
    }

    if(index_file_read.is_open()) index_file_read.close();
}

StagingArea::~StagingArea() {
    if(index_file_read.is_open()) index_file_read.close();
    if(index_file_write.is_open()) index_file_write.close();
}

void StagingArea::update_or_add_entry(const std::filesystem::path& file_path, const std::string& obj_mode, const std::string& obj_hash) {
    auto [it, inserted] = entries.try_emplace(file_path.string(), index_entry(obj_mode, obj_hash));
    
    if (!inserted) {
        // Entry already existed, update it
        it->second.obj_mode = obj_mode;
        it->second.obj_hash = obj_hash;
    }
}

void StagingArea::delete_entry(std::filesystem::path& deleted_file_path) {
    entries.erase(deleted_file_path.string());
}

void StagingArea::write_to_index_file(const std::filesystem::path& index_file_path) {
    std::cout << index_file_path << std::endl;
    if (!std::filesystem::exists(index_file_path.parent_path())) {
        throw std::runtime_error("File: stage.cpp\nFunction: write_to_index_file\nError: Directory does not exist");
    }

    index_file_write.open(index_file_path, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!index_file_write.is_open()) {
        throw std::runtime_error("File: stage.cpp\nFunction: write_to_index_file\nError: Failed to open index file for writing");
    }

    // Write all entries to the file
    for (const auto& entry : entries) {
        index_file_write << entry.second.obj_mode << " " 
                         << entry.second.obj_hash << " " 
                         << entry.first << "\n";
    }

    // Flush and close the file
    index_file_write.flush();
    index_file_write.close();

    if (index_file_write.fail()) {
        throw std::runtime_error("File: stage.cpp\nFunction: write_to_index_file\nError: Failed to write to index file");
    }
}

std::string StagingArea::to_string() const {
    std::ostringstream oss;
    
    for (const auto& [path, entry] : entries) {
        oss << entry.obj_mode << " " << entry.obj_hash << " " << path << "\n";
    }
    
    return oss.str();
}
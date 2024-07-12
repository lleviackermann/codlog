#include <helper.h>
#include <string>
#include <ostream>
#include <filesystem>

TreeEntry::TreeEntry(const std::string &mode, const std::string &obj_type, const std::string &hash, const std::string &name) : obj_mode(mode), obj_type(obj_type), obj_hash(hash), obj_name(name) {}

std::string get_initialised_repo(const std::string &target_dir)
{
    std::filesystem::path target_dir_path = target_dir;

    // Iterate up the directory hierarchy from the target directory to the root
    for (std::filesystem::path current_dir = target_dir_path; current_dir.has_relative_path(); current_dir = current_dir.parent_path())
    {
        std::filesystem::path codlog_dir = current_dir / ".codlog";
        if (std::filesystem::exists(codlog_dir))
        {
            return current_dir.string();
        }
    }

    throw std::invalid_argument("Error: not a codlog repository (or any parent up to mount point /).\nTo initialize a repo run the command `codlog init [directory_name]`\n");
    return "";
}

std::ostream &operator<<(std::ostream &os, FileMode mode)
{
    switch (mode)
    {
    case FileMode::EXECUTABLE:
        return os << "EXECUTABLE (" << static_cast<int>(mode) << ")";
    case FileMode::REGULAR:
        return os << "REGULAR (" << static_cast<int>(mode) << ")";
    case FileMode::DIRECTORY:
        return os << "DIRECTORY (" << static_cast<int>(mode) << ")";
    default:
        return os << "UNKNOWN (" << static_cast<int>(mode) << ")";
    }
}

std::ostream &operator<<(std::ostream &os, const TreeEntry &entry)
{
    os << "     Mode: " << entry.obj_mode << "\n";
    os << "     Type: " << entry.obj_type << "\n";
    os << "     Hash: " << entry.obj_hash << "\n";
    os << "     Name: " << entry.obj_name << "\n";
    return os;
}

std::string getFileModeStr(FileMode mode)
{
    switch (mode)
    {
    case FileMode::EXECUTABLE:
        return "100755";
    case FileMode::REGULAR:
        return "100644";
    case FileMode::DIRECTORY:
        return "040000";
    default:
        return "000000";
    }
}

std::vector<unsigned char> convertHexToBinary(std::string &hash)
{
    std::vector<unsigned char> binaryContent;
    for (unsigned int i = 0; i < hash.length(); i += 2)
    {
        std::string byteString = hash.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), NULL, 16));
        binaryContent.push_back(byte);
    }
    return binaryContent;
}
// #ifndef COMMIT_OBJ_H
// #define COMMIT_OBJ_H

// #include <string>
// #include <vector>
// #include <unordered_map>
// #include <filesystem>

// class Commit {
// public:
//     Commit();
//     Commit(const std::string& message, const std::string& author, const std::string& parent_hash);

//     void set_tree_hash(const std::string& hash);
//     void add_file(const std::string& path, const std::string& hash, const std::string& mode);
//     std::string generate_hash() const;
//     std::string serialize() const;
//     static Commit deserialize(const std::string& serialized_data);

//     std::string get_message() const;
//     std::string get_author() const;
//     std::string get_parent_hash() const;
//     std::string get_tree_hash() const;
//     const std::unordered_map<std::string, std::pair<std::string, std::string>>& get_files() const;

// private:
//     std::string message;
//     std::string author;
//     std::string parent_hash;
//     std::string tree_hash;
//     std::unordered_map<std::string, std::pair<std::string, std::string>> files; // path -> (hash, mode)
    
//     std::string calculate_hash() const;
// };


// #endif
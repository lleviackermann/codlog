// #include <commit_obj.h>
// #include <sstream>
// #include <iomanip>
// #include <chrono>
// #include <openssl/sha.h>

// Commit::Commit() {}

// Commit::Commit(const std::string& message, const std::string& author, const std::string& parent_hash)
//     : message(message), author(author), parent_hash(parent_hash) {}

// void Commit::set_tree_hash(const std::string& hash) {
//     tree_hash = hash;
// }

// void Commit::add_file(const std::string& path, const std::string& hash, const std::string& mode) {
//     files[path] = std::make_pair(hash, mode);
// }

// std::string Commit::generate_hash() const {
//     return calculate_hash();
// }

// std::string Commit::serialize() const {
//     std::ostringstream oss;
//     oss << "tree " << tree_hash << "\n";
//     oss << "parent " << parent_hash << "\n";
//     oss << "author " << author << "\n";
//     oss << "message " << message << "\n";
    
//     for (const auto& [path, info] : files) {
//         oss << info.second << " " << info.first << " " << path << "\n";
//     }

//     return oss.str();
// }

// Commit Commit::deserialize(const std::string& serialized_data) {
//     std::istringstream iss(serialized_data);
//     std::string line;
//     Commit commit;

//     while (std::getline(iss, line)) {
//         std::istringstream line_stream(line);
//         std::string key, value;
//         line_stream >> key;

//         if (key == "tree") {
//             line_stream >> commit.tree_hash;
//         } else if (key == "parent") {
//             line_stream >> commit.parent_hash;
//         } else if (key == "author") {
//             std::getline(line_stream, commit.author);
//             commit.author = commit.author.substr(1); // Remove leading space
//         } else if (key == "message") {
//             std::getline(line_stream, commit.message);
//             commit.message = commit.message.substr(1); // Remove leading space
//         } else {
//             // This is a file entry
//             std::string mode, hash, path;
//             line_stream >> hash >> path;
//             commit.files[path] = std::make_pair(hash, key); // key here is the mode
//         }
//     }

//     return commit;
// }

// std::string Commit::get_message() const { return message; }
// std::string Commit::get_author() const { return author; }
// std::string Commit::get_parent_hash() const { return parent_hash; }
// std::string Commit::get_tree_hash() const { return tree_hash; }
// const std::unordered_map<std::string, std::pair<std::string, std::string>>& Commit::get_files() const { return files; }

// std::string Commit::calculate_hash() const {
//     std::string content = serialize();
//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, content.c_str(), content.length());
//     SHA256_Final(hash, &sha256);

//     std::stringstream ss;
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//         ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
//     }

//     return ss.str();
// }
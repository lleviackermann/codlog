int make_tree_and_store(std::unordered_map<std::filesystem::path, TreeEntry>& obj_info, const std::filesystem::path& cur_dir, const std::filesystem::path& repo_path) {
    // std::cout << cur_dir << "\n";
    // return 0;
    if(cur_dir.filename() == ".codlog" || cur_dir.filename() == ".git") return 0;
    int file_no = 0;
    Tree cur_tree;
    for(const auto& cur_path : std::filesystem::directory_iterator(cur_dir)) {
        const std::filesystem::path cur_path_relative = std::filesystem::relative(cur_path, repo_path);
        if(std::filesystem::is_directory(cur_path)) {
            // std::cout << cur_dir << " " << cur_path << "\n";
            int cnt = make_tree_and_store(std::ref(obj_info), cur_path, repo_path);
            file_no += cnt;
            if(cnt != 0) {
                cur_tree.addEntry(obj_info[cur_path_relative]);
            }
        } else {
            cur_tree.addEntry(obj_info[cur_path_relative]);
            file_no++;
        }
    }
    if(file_no) {
        std::filesystem::path obj_directory = repo_path / ".codlog" / "objects";
        TreeEntry cur_tree_entry(tree_mode, tree_def, cur_tree.calculateHash(), cur_dir.filename());
        std::filesystem::path cur_dir_relative = std::filesystem::relative(cur_dir, repo_path);
        cur_tree.writeToFile(obj_directory);
        if(cur_dir == repo_path) cur_dir_relative = "./";
        obj_info[cur_dir_relative] = std::move(cur_tree_entry);
    }
    return file_no;
}
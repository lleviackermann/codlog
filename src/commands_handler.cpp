#include <commands_handler.h>
#include <helper.h>
#include <init.h>
#include <add.h>
#include <iostream>
#include <string>

int CommandsHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "init") {
        init_command(args);
    } else if (command == "add") {
        std::string initialized_repo = get_initialised_repo(args.back());
        if(initialized_repo == "") {
            throw std::invalid_argument("Error: not a codlog repository (or any parent up to mount point /).\nTo initialize a repo run the command `codlog init [directory_name]`\n");
        }
        add_command(args, initialized_repo);
    // } else if (command == "commit") {
    //     commit(args);
    // } else if (command == "status") {
    //     status(args);
    // } else if (command == "log") {
    //     log(args);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
    return 0;
}
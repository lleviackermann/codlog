#include <commands_handler.h>
#include <helper.h>
#include <init.h>
#include <add.h>
#include <commit_cmd.h>
#include <status.h>
#include <iostream>
#include <string>

int CommandsHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "init") {
        init_command(args);
    } else if (command == "add") {
        std::string initialized_repo = get_initialised_repo(args.back());
        add_command(args, initialized_repo);
    } else if (command == "commit") {
        std::string initialized_repo = get_initialised_repo(args.back());
        commit_command(args, initialized_repo);
    } else if (command == "status") {
        std::string initialized_repo = get_initialised_repo(args.back());
        status_command(args, initialized_repo);
    // } else if (command == "log") {
    //     log(args);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
    return 0;
}
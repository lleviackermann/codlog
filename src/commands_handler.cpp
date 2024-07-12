#include <commands_handler.h>
#include <helper.h>
#include <init.h>
#include <add.h>
#include <commit_cmd.h>
#include <status.h>
#include <config_cmd.h>
#include <iostream>
#include <string>

int CommandsHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    std::string initialized_repo = "";
    if(command != "init") initialized_repo = get_initialised_repo(args.back());
    if (command == "init") {
        init_command(args);
    } else if (command == "add") {
        add_command(args, initialized_repo);
    } else if (command == "commit") {
        commit_command(args, initialized_repo);
    } else if (command == "status") {
        status_command(args, initialized_repo);
    } else if (command == "config") {
        config_command(args);
    // } else if (command == "log") {
    //     log(args);
    // } 
    } else if (command == "commit") {
        commit_command(args, initialized_repo);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
    return 0;
}
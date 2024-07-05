#include <commands_handler.h>
#include <init.h>
#include <iostream>
#include <string>

int CommandsHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "init") {
        init_command(args);
    // } else if (command == "add") {
    //     add(args);
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
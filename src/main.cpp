#include <commands_handler.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: codlog <command> [<args>...]" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::vector<std::string> args(argv + 2, argv + argc);

    CommandsHandler::executeCommand(command, args);
    return 0;
}
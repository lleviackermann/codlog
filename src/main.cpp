#include <commands_handler.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: codlog <command> [<args>...]" << std::endl;
            return 1;
        }

        std::string command = argv[1];
        std::vector<std::string> args(argv + 2, argv + argc);

        CommandsHandler::executeCommand(command, args);
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE; // Exit the program with a failure code
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE; // Exit the program with a failure code
    }
    return EXIT_SUCCESS; // Exit the program with a success code
}
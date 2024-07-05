#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <string>
#include <vector>

class CommandsHandler {
public:
    static int executeCommand(const std::string& command, const std::vector<std::string>& args);

// private:
    // static int init(const std::vector<std::string>& args);
    // static int add(const std::vector<std::string>& args);
    // static int commit(const std::vector<std::string>& args);
    // static int status(const std::vector<std::string>& args);
    // static int log(const std::vector<std::string>& args);
};

#endif 
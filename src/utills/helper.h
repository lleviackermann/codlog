#ifndef HELPER_H
#define HELPER_H

#include <filemode.h>
#include <string>
#include <ostream>

std::string get_initialised_repo(const std::string& target_dir);
std::ostream& operator<<(std::ostream& os, FileMode mode);

#endif
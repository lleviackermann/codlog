#ifndef FILEMODE_H
#define FILEMODE_H

enum class FileMode {
    EXECUTABLE = 100755,
    REGULAR = 100644,
    DIRECTORY = 040000
};

#endif
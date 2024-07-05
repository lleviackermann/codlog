# Directory where all the source codes are present
SRC_DIR = $(CURDIR)/src

# Directory containing all the objects
OBJECTS_DIR = $(SRC_DIR)/objects

# Directory containing all the commands
COMMANDS_DIR = $(SRC_DIR)/commands

# Main cpp file
MAIN_FILE = $(SRC_DIR)/main

# File for executing different commands
COMMANDS_HANDLER = $(SRC_DIR)/commands_handler

# All objects files
TREE_FILE = $(OBJECTS_DIR)/tree
BLOB_FILE = $(OBJECTS_DIR)/blob
COMMIT_OBJ_FILE = $(OBJECTS_DIR)/commit

# All commands file
ADD_FILE = $(COMMANDS_DIR)/add
COMMIT_CMD_FILE = $(COMMANDS_DIR)/commit
INIT_FILE = $(COMMANDS_DIR)/init
LOG_FILE = $(COMMANDS_DIR)/log
STATUS_FILE = $(COMMANDS_DIR)/status

CC = g++
CFLAGS = -Wall -g -std=c++17
LDFLAGS =

# Source files
SRCS = $(MAIN_FILE).cpp $(COMMANDS_HANDLER).cpp $(TREE_FILE).cpp $(BLOB_FILE).cpp $(COMMIT_OBJ_FILE).cpp $(ADD_FILE).cpp $(COMMIT_CMD_FILE).cpp $(INIT_FILE).cpp $(LOG_FILE).cpp $(STATUS_FILE).cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files directory
INCLUDES = -I$(SRC_DIR) -I$(OBJECTS_DIR) -I$(COMMANDS_DIR)


.PHONY: init_command add_command status_command commit_command log_command clean

# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Dependencies
$(MAIN_FILE).o: $(MAIN_FILE).cpp $(COMMANDS_HANDLER).h
$(COMMANDS_HANDLER).o: $(COMMANDS_HANDLER).cpp $(COMMANDS_HANDLER).h
$(TREE_FILE).o: $(TREE_FILE).cpp $(TREE_FILE).h
$(BLOB_FILE).o: $(BLOB_FILE).cpp $(BLOB_FILE).h
$(COMMIT_OBJ_FILE).o: $(COMMIT_OBJ_FILE).cpp $(COMMIT_OBJ_FILE).h
$(ADD_FILE).o: $(ADD_FILE).cpp $(ADD_FILE).h
$(COMMIT_CMD_FILE).o: $(COMMIT_CMD_FILE).cpp $(COMMIT_CMD_FILE).h
$(INIT_FILE).o: $(INIT_FILE).cpp $(INIT_FILE).h
$(LOG_FILE).o: $(LOG_FILE).cpp $(LOG_FILE).h
$(STATUS_FILE).o: $(STATUS_FILE).cpp $(STATUS_FILE).h

# Common files which are required to compile for each command
COMMON_FILES = $(MAIN_FILE).o $(COMMANDS_HANDLER).o

# All objects files
OBJECTS_FILES = $(TREE_FILE).o $(COMMIT_OBJ_FILE).o $(BLOB_FILE).o

# Rules for commands
init_command: $(COMMON_FILES) $(INIT_FILE).o
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMMON_FILES) $(INIT_FILE).o -o $(CURDIR)/init_command

add_command: $(COMMON_FILES) $(ADD_FILE).o
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMMON_FILES) $(ADD_FILE).o -o $(CURDIR)/add_command

status_command: $(COMMON_FILES) $(STATUS_FILE).o
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMMON_FILES) $(STATUS_FILE).o -o $(CURDIR)/status_command

commit_command: $(COMMON_FILES) $(COMMIT_CMD_FILE).o
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMMON_FILES) $(COMMIT_CMD_FILE).o -o $(CURDIR)/commit_command

log_command: $(COMMON_FILES) $(LOG_FILE).o
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMMON_FILES) $(LOG_FILE).o -o $(CURDIR)/log_command

# Clean rule
clean:
	rm -f $(OBJS) $(SRC_DIR)/myvcs $(COMMANDS_DIR)/*_command $(CURDIR)/*_command

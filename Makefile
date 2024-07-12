# Directory where all the source codes are present
SRC_DIR = $(CURDIR)/src

# Directory containing all the objects
OBJECTS_DIR = $(SRC_DIR)/objects

# Directory containing all the commands
COMMANDS_DIR = $(SRC_DIR)/commands

# Utills Directory
UTILLS_DIR = $(SRC_DIR)/utills

# Main cpp file
MAIN_FILE = $(SRC_DIR)/main

# File for executing different commands
COMMANDS_HANDLER = $(SRC_DIR)/commands_handler

# All objects files
# BASE_OBJ_FILE = $(OBJECTS_DIR)/base_obj
TREE_FILE = $(OBJECTS_DIR)/tree
BLOB_FILE = $(OBJECTS_DIR)/blob
COMMIT_OBJ_FILE = $(OBJECTS_DIR)/commit_obj

# All commands file
ADD_FILE = $(COMMANDS_DIR)/add
COMMIT_CMD_FILE = $(COMMANDS_DIR)/commit_cmd
INIT_FILE = $(COMMANDS_DIR)/init
LOG_FILE = $(COMMANDS_DIR)/log
STATUS_FILE = $(COMMANDS_DIR)/status
CONFIG_CMD_FILE = $(COMMANDS_DIR)/config_cmd

# All Utills file
HELPER_FILE = $(UTILLS_DIR)/helper
STAGE_FILE = $(UTILLS_DIR)/stage
CONFIG_UTILLS_FILE = $(UTILLS_DIR)/config

CC = g++
CFLAGS = -Wall -g -std=c++17
LDFLAGS = -lssl -lcrypto

# Source files
SRCS = $(MAIN_FILE).cpp $(COMMANDS_HANDLER).cpp $(TREE_FILE).cpp $(BLOB_FILE).cpp $(COMMIT_OBJ_FILE).cpp $(ADD_FILE).cpp $(COMMIT_CMD_FILE).cpp $(INIT_FILE).cpp $(LOG_FILE).cpp $(STATUS_FILE).cpp $(HELPER_FILE).cpp $(STAGE_FILE).cpp $(CONFIG_UTILLS_FILE).cpp $(CONFIG_CMD_FILE).cpp 

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files directory
INCLUDES = -I$(SRC_DIR) -I$(OBJECTS_DIR) -I$(COMMANDS_DIR) -I$(UTILLS_DIR)


.PHONY: run_command clean

# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(LDFLAGS)

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
$(HELPER_FILE).o: $(HELPER_FILE).cpp $(HELPER_FILE).h
$(STAGE_FILE).o: $(STAGE_FILE).cpp $(STAGE_FILE).h
$(CONFIG_UTILLS_FILE).o: $(CONFIG_UTILLS_FILE).cpp $(CONFIG_UTILLS_FILE).h
$(CONFIG_CMD_FILE).o: $(CONFIG_CMD_FILE).cpp $(CONFIG_CMD_FILE).h
# $(BASE_OBJ_FILE).o: $(CONFIG_CMD_FILE).cpp $(CONFIG_CMD_FILE).h


# Rules for commands

run_command: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(CURDIR)/run_command $(LDFLAGS)


# Clean rule

clean:
	rm -f $(OBJS) $(SRC_DIR)/myvcs $(CURDIR)/*_command

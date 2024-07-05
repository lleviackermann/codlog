#!/bin/bash

# Check if the first argument is provided
if [ "$#" -lt 1 ]; then
    echo "No argument provided"
    exit 1
fi

current_dir=$(dirname "$(readlink -f "$0")")
# Check if the first argument is "add"
if [ "$1" == "init" ]; then
    cd $current_dir
    make init_command
    ./init_command $@
    make clean
else
    echo "Invalid argument. Nothing to do."
fi

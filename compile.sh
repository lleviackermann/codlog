#!/bin/bash

# Check if the first argument is provided
if [ "$#" -lt 1 ]; then
    echo "No argument provided"
    exit 1
fi

target_dir=$(pwd)
current_dir=$(dirname "$(readlink -f "$0")")

cd $current_dir
make run_command
./run_command $@ $target_dir
make clean
cd $target_dir
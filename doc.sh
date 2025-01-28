#!/bin/bash

# Check if the input file is provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <C++_file>"
    exit 1
fi

# Assign the provided file name to a variable
CPP_FILE="$1"

# Check if the file exists
if [ ! -f "$CPP_FILE" ]; then
    echo "Error: The file '$CPP_FILE' was not found."
    exit 1
fi

# Use clang++ to generate AST and extract function declarations, including templates
clang++ -Xclang -ast-dump -fsyntax-only "$CPP_FILE" | grep "FunctionDecl" | awk '{print $4, $5, $6, $7}'


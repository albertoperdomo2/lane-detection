#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

# remove cached files
if rm -r CMake*; then
 echo "deleting old cmake files..."
else
 echo "no cmake cached files found!"
fi

cmake ../ && cmake --build .

#!/bin/bash
OUTPUT_PATH="./DriverExecutable"
echo "Compiling sources"
g++ -o $OUTPUT_PATH src/Driver.cpp src/User.cpp 
sleep 3
echo "Running the executable..."
"$OUTPUT_PATH"

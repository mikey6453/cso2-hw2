#!/bin/bash

# Initialize variables
total_time=0
run_count=1000

# Ensure the program is compiled
make gettimings

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed, exiting..."
    exit 1
fi

# Run the program 1000 times
for i in $(seq 1 $run_count); do
    # Capture the output of the program
    output=$(./gettimings 1)
    
    # Add the output to total_time
    total_time=$((total_time + output))
done

# Calculate the average
average=$((total_time / run_count))

# Print the average time
echo "Average time over $run_count runs: $average ns"

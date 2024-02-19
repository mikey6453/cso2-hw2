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
    # Capture the output of the program, assuming it prints something like "Average time: X ns"
    output=$(./gettimings 1)
    
    # Extract just the numeric part of the timing from the output
    # Assuming the numeric timing is followed by "ns" and is the last occurrence of a number in the line
    time=$(echo $output | grep -o '[0-9]\+' | tail -1)
    
    # Add the extracted time to total_time
    total_time=$((total_time + time))
done

# Calculate the average
average=$((total_time / run_count))

# Print the average time
echo "Average time over $run_count runs: $average ns"

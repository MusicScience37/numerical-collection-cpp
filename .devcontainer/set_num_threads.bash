#!/bin/bash

# Get the number of physical CPU cores
NUM_PHYSICAL_CORES=$(lscpu | awk '/^Socket\(s\):/ {sockets=$2} /^Core\(s\) per socket:/ {cores=$4} END {print sockets * cores}')

# Export OMP_NUM_THREADS
export OMP_NUM_THREADS=$NUM_PHYSICAL_CORES

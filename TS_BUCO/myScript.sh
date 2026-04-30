#!/bin/bash

# Check if exactly 3 arguments are passed
if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <Type of the instance> <Number of categories> <Number of items>"
  echo "Example: $0 CO 2 100"
  exit 1
fi

prefix="$1"
v1="$2"
v2="$3"

k="${prefix}_${v1}_${v2}"

# Get the path of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Navigate to the instance folder
INSTANCE_FOLDER="${SCRIPT_DIR}/../Instance_$k"
RESULTS_FOLDER="Results_$k"

# Create the results folder if it doesn't exist
mkdir -p "$RESULTS_FOLDER"

# Run script on each of the 30 files
for i in {1..30}; do
  ./mainTS "${INSTANCE_FOLDER}/instance_$i.txt" "${RESULTS_FOLDER}/res_$i.txt" >> "log_$k.txt"
done
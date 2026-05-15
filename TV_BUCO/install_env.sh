#!/bin/bash
# This script will set up the Python environment based on requirements.txt

# Create a virtual environment
python3 -m venv venv

# Activate the virtual environment
source venv/bin/activate

# Upgrade pip
pip3 install --upgrade pip

# Install dependencies
pip3 install -r requirements.txt
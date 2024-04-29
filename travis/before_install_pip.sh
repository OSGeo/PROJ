#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

# Configure Python pip
python3 -m pip config --user set global.progress_bar off
python3 -m pip install --user --upgrade pip
python3 -m pip install --user jsonschema pyyaml pytest

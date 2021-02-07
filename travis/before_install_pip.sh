#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

# Configure Python pip
python3 -m pip install --user --upgrade "pip < 21.0"
echo `python3 -m pip --version`
python3 -m pip config --user set global.progress_bar off
python3 -m pip install --user jsonschema

#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

# Configure Python pip
pip3 install --user --upgrade "pip < 21.0"
echo `pip3 --version`
pip3 config --user set global.progress_bar off
pip3 install --user jsonschema

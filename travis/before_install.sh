#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

# Configure Python pip
pip install --user --upgrade pip
pip config --user set global.progress_bar off

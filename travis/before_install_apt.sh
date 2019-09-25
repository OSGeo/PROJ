#!/bin/bash

# All Debian-like before_install scripts starts by running this
# "global" before_install script.

sudo apt-get update -qq
sudo apt-get install -qq python3-pip python3-setuptools

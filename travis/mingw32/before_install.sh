#!/bin/bash

set -e

./travis/before_install.sh

sudo apt-get update -qq
sudo apt-get install -qq wine
sudo apt-get install -qq mingw32

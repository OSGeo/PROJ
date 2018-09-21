#!/bin/bash

set -e

./travis/before_install.sh

sudo apt-get update -qq
sudo apt-get install wget
sudo apt-get install binutils-mingw-w64-x86-64
sudo apt-get install gcc-mingw-w64-x86-64
sudo apt-get install g++-mingw-w64-x86-64
sudo apt-get install g++-mingw-w64
sudo apt-get install mingw-w64-tools
sudo apt-get install -y wine1.4-amd64
sudo apt-get install sqlite3
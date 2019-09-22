#!/bin/bash

set -e
sudo apt-get update -qq
sudo apt-get install -qq python3-pip

./travis/before_install.sh

sudo apt-get install -qq sqlite3 libsqlite3-dev
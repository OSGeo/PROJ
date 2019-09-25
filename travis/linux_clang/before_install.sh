#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq sqlite3 libsqlite3-dev

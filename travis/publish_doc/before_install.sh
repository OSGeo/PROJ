#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh
./travis/docker.sh

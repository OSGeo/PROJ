#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
    sqlite3 zip \
    libsqlite3-dev \
    libtiff-dev \
    libcurl4-openssl-dev libnghttp2-dev libidn2-dev librtmp-dev libssh-dev \
      libpsl-dev libssl-dev libkrb5-dev comerr-dev libldap2-dev libbrotli-dev

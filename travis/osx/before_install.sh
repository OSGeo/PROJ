#!/bin/bash

set -e

sudo easy_install pip

./travis/before_install.sh

brew update
brew install ccache

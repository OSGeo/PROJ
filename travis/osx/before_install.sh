#!/bin/bash

set -e

./travis/before_install.sh

brew install ccache
brew install python3

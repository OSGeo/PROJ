#!/bin/bash

set -e

./travis/before_install.sh

brew update
brew install ccache

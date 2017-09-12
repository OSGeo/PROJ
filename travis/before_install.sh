#!/bin/bash

# Specify python version. This introduced because the
# Travis setup seems to vary between platforms.
pyenv global system $PY_VERSION
pyenv versions # a bit of debug info

pip install --user cpp-coveralls
./travis/docker.sh



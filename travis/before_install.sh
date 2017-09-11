#!/bin/bash

pyenv global system 3.6
pip install --user cpp-coveralls
./travis/docker.sh



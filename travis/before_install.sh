#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

# Specify which version of python to use. The default python to use on Travis
# can vary from platform to platform, so we use pyenv to make sure we use
# a version that works for the complete build processs.
#pyenv global system 3.5
#pyenv versions # a bit of debug info

which python
python --version
which python3
python3 --version
#which python35
#python3.5 --version

#ls /opt/pyenv/shims

which pip
pip --version
#which pip3
#pip3 --version

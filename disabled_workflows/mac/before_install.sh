#!/bin/bash

set -e


conda update -n base -c defaults conda
conda install compilers -y

conda config --set channel_priority strict
conda install --yes --quiet python=3.8 autoconf automake libtool ccache -y
conda install --yes --quiet proj=7.1.1=h45baca5_3 --only-deps -y

./travis/before_install_pip.sh

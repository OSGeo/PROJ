#!/bin/bash

set -e


conda update --yes --quiet -n base -c defaults conda
conda install --yes --quiet compilers

conda config --set channel_priority strict
conda install --yes --quiet python=3.8 autoconf automake libtool ccache jsonschema
conda install --yes --quiet --only-deps proj=7.1.1=h45baca5_3

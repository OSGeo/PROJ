#!/bin/bash

set -e

mamba install --yes --quiet compilers
mamba install --yes --quiet python=3.8 autoconf automake libtool ccache jsonschema
mamba install --yes --quiet --only-deps proj=7.1.1=h45baca5_3

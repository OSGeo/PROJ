#!/bin/sh
set -e

CONDA_PREFIX="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/conda/$READTHEDOCS_VERSION"

export PATH="$CONDA_PREFIX/bin:$PATH"
export PROJ_DATA="$CONDA_PREFIX/share/proj"

cd docs/plot
python plot.py plotdefs.json images
cd ../..

mkdir -p docs/source/operations/projections/images/
cp -r docs/plot/images/* docs/source/operations/projections/images/

#!/bin/sh
set -e

export PATH="$HOME/.local/bin:$PATH"
export PROJ_DATA="$HOME/.local/share/proj"

cd docs/plot
python plot.py plotdefs.json images
cd ../..

mkdir -p docs/source/operations/projections/images/
cp -r docs/plot/images/* docs/source/operations/projections/images/

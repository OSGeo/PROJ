#!/bin/bash

mkdir packages

conda build recipe --clobber-file recipe/recipe_clobber.yaml --output-folder packages
conda install -c ./packages proj

#projinfo -s NAD27 -t EPSG:4269 --area "USA - Missouri"

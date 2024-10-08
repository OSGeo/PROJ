#!/bin/bash

conda update -n base -c defaults conda -y
conda install conda-build ninja compilers -y

pwd
ls
git clone https://github.com/conda-forge/proj.4-feedstock.git

cd proj.4-feedstock
cat > recipe/recipe_clobber.yaml <<EOL
source:
  path: ../../../PROJ
  url:
  sha256:
  patches:

build:
  number: 2112
EOL

ls recipe

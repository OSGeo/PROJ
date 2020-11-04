#!/bin/bash

conda update -n base -c defaults conda -y
conda install conda-build ninja compilers -y

pwd
ls
git clone https://github.com/conda-forge/proj.4-feedstock.git

cd proj.4-feedstock
cat > recipe/recipe_clobber.yaml <<EOL
source:
  git_url: https://github.com/OSGeo/PROJ.git
  git_rev: ${GITHUB_SHA}
  url:
  sha256:

build:
  number: 2112
EOL

ls recipe

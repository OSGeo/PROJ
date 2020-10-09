#!/bin/bash

conda update -n base -c defaults conda -y

if [ "$PLATFORM" == "ubuntu-latest" ]; then
    conda install conda-build ninja compilers automake libtool -y
    ./autogen.sh
else
    conda install conda-build ninja compilers -y
fi


pwd
ls
git clone https://github.com/conda-forge/proj.4-feedstock.git

cd proj.4-feedstock
cat > recipe/recipe_clobber.yaml <<EOL
source:
  git_url: https://github.com/hobu/PROJ.git
  git_rev: ${GITHUB_SHA}
  url:
  sha256:

build:
  number: 2112
EOL

ls recipe

#!/bin/bash

set -e

./travis/before_install.sh

brew update
brew install ccache
brew install sqlite3
brew install doxygen

export PATH=$HOME/Library/Python/2.7/bin:$PATH
pip install --user sphinx sphinx-rtd-theme sphinxcontrib-bibtex breathe
which sphinx-build
(cd docs; make html)

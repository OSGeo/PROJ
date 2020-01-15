#!/bin/bash

set -e

export PATH=$HOME/Library/Python/3.7/bin:$PATH

brew update
brew install ccache
#brew upgrade sqlite3
#brew upgrade libtiff
brew install doxygen
#brew install md5sha1sum
#brew reinstall python
brew reinstall wget

./travis/before_install_pip.sh

pip3 install --user sphinx sphinx-rtd-theme sphinxcontrib-bibtex breathe
which sphinx-build

(cd docs; make html)

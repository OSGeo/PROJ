#!/bin/bash

set -e

./travis/before_install.sh

brew update
brew install ccache
brew install sqlite3
brew install doxygen
brew install md5sha1sum

export PATH=$HOME/Library/Python/2.7/bin:$PATH
# breathe=4.12.0 is the last version to work for us with sphinx 1.8.5 / Python 2
pip install --user sphinx sphinx-rtd-theme sphinxcontrib-bibtex breathe==4.12.0
which sphinx-build
(cd docs; make html)

#!/bin/bash

set -e

export PATH=$HOME/Library/Python/3.7/bin:$PATH

./travis/before_install.sh

brew update
brew install ccache
brew install sqlite3
brew install doxygen
brew install md5sha1sum
brew reinstall python
brew reinstall wget

pip3 install --user sphinx sphinx-rtd-theme sphinxcontrib-bibtex breathe
which sphinx-build
(cd docs; make html)

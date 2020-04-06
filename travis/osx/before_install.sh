#!/bin/bash

set -e

export PATH=$HOME/Library/Python/3.7/bin:$PATH

brew update
brew install ccache
brew install sqlite3
brew install doxygen
#brew install md5sha1sum
#brew reinstall python
brew reinstall wget

./travis/before_install_pip.sh

pip3 install --user sphinx==2.4.4 sphinx-rtd-theme sphinxcontrib-bibtex breathe==4.14.0
which sphinx-build

(cd docs; make html)

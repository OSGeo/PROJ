#!/bin/sh
set -eu

# Refuse to reformat nn.hpp: this is third-party code
if test $(basename $1) = "nn.hpp"; then
    exit 0
fi

clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}"  $1 > $1.reformatted
if diff -u $1.reformatted $1; then
    # No reformatting: remove temporary file
    rm $1.reformatted
else
    # Differences. Backup original file, and use reformatted file
    cp $1 $1.before_reformat
    mv $1.reformatted $1
fi

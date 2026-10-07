#!/bin/bash

mkdir packages

CONDA_PLAT=""
if grep -q "windows" <<< "$GHA_CI_PLATFORM"; then
    CONDA_PLAT="win"
    ARCH="64"
fi

if grep -q "ubuntu" <<< "$GHA_CI_PLATFORM"; then
    CONDA_PLAT="linux"
    ARCH="64"
fi

if grep -q "macos-15-intel" <<< "$GHA_CI_PLATFORM"; then
    CONDA_PLAT="osx"
    ARCH="64"
elif grep -q "macos-15" <<< "$GHA_CI_PLATFORM"; then
    CONDA_PLAT="osx"
    ARCH="arm64"
fi

rattler-build build \
    --recipe-dir recipe \
    --output-dir packages \
    --variant-config ".ci_support/${CONDA_PLAT}_${ARCH}_.yaml"
conda install -c ./packages proj

#projinfo -s NAD27 -t EPSG:4269 --area "USA - Missouri"

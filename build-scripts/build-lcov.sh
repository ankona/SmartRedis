#!/bin/bash

# Install LCOV
if [[ -f ./lcov/install/usr/local/bin/lcov ]]; then
    echo "LCOV has already been download and installed"
else
    echo "Installing LCOV"
    if [[ ! -d "./lcov" ]]; then
        git clone https://github.com/linux-test-project/lcov.git lcov
        cd lcov
        git checkout tags/v1.15
        cd ..
    else
        echo "LCOV downloaded"
    fi
    cd lcov
    mkdir install
    echo "Building LCOV v1.15"
    CC=gcc CXX=g++ DESTDIR="install/" make install
    cd ../
fi
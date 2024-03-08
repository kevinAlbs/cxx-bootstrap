#!/usr/bin/env bash
set -o errexit

# Include path to C driver install
export CMAKE_PREFIX_PATH=$HOME/code/c-bootstrap/install/mongo-c-driver-1.26.0

cd KEVINALBS
if [ ! -d mongo-cxx-driver ]; then
    git clone git@github.com:mongodb/mongo-cxx-driver.git mongo-cxx-driver
fi

cd mongo-cxx-driver
git checkout r3.8.1
# Install C++ driver 3.8.1
cmake \
    -DBUILD_VERSION=3.8.1 \
    -DCMAKE_INSTALL_PREFIX=../install-3.8.1 \
    -S . -B cmake-build-3.8.1

cmake --build cmake-build-3.8.1 --target install

git checkout r3.9.0
# Install C++ driver 3.9.0
cmake \
    -DBUILD_VERSION=3.9.0 \
    -DCMAKE_INSTALL_PREFIX=../install-3.9.0 \
    -S . -B cmake-build-3.9.0

cmake --build cmake-build-3.9.0 --target install

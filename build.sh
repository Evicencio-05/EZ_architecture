#!/bin/bash

# Build script for EZ_Architecture

BUILD_DIR="build"
BUILD_TYPE="${1:-Debug}"

echo "Building EZ_Architecture in ${BUILD_TYPE} mode..."

# Create build directory
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Run CMake
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

# Build
make -j$(nproc)

echo "Build complete! Executables are in ${BUILD_DIR}/bin/"

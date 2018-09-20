#!/bin/bash

echo "=== Building PPar metrics collector library! ==="

PROJECT_DIR="${PWD}"
BUILD_DIR="${PROJECT_DIR}/build"
C_FLAGS=""

if [[ -e ${BUILD_DIR} ]]; then
    rm -rf ${BUILD_DIR}
fi
mkdir ${BUILD_DIR}

echo "= CMake is generating build system ="
cd ${BUILD_DIR}
CC=gcc CXX=g++ cmake \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="${C_FLAGS}" \
    "${PROJECT_DIR}"

echo "= Building PPar metrics tool ="
cmake --build .

cd ${PROJECT_DIR}

echo "=== PPar metrics collector library build script finished! ==="

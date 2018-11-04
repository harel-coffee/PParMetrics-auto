#!/bin/bash

source $(dirname "$0")/../project-environment.sh

echo "< build-ppar-metrics-so-library.sh script >"
echo "=> Building PPar tool *.so library"

if [[ -e ${PPAR_TOOL_BUILD_DIR} ]]; then
    rm -rf ${PPAR_TOOL_BUILD_DIR}
fi
mkdir ${PPAR_TOOL_BUILD_DIR}

C_FLAGS=""

(
cd ${PPAR_TOOL_BUILD_DIR}
echo "=> CMake is generating build system"
CC=gcc CXX=g++ cmake \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="${C_FLAGS}" \
    "${PROJECT_ROOT_DIR}"
echo "=> Building PPar metrics tool"
cmake --build .
)

echo "=> PPar library build script finished!"

#!/bin/bash

echo "=== Benchmarks prebuilding script! ==="

SRC_DIR="${PWD}"
BENCHMARKS_DIR="${SRC_DIR}/benchmarks"
NAS_BENCHMARK_SOURCES_DIR="${SRC_DIR}/benchmarks/SNU_NPB/"
NAUSEOUS_CMAKE_BUILD_HARNESS_DIR="${SRC_DIR}/benchmarks/nauseous/"
BENCHMARKS_BUILD_DIR="${SRC_DIR}/benchmarks-build"
C_FLAGS=""

if [[ -e ${BENCHMARKS_BUILD_DIR} ]]; then
    rm -rf ${BENCHMARKS_BUILD_DIR}
fi
mkdir ${BENCHMARKS_BUILD_DIR}

# create symbolic links from the nauseous NAS benchmark build harness to NAS benchmark C source code
echo "= Creating symbolic links form NAS CMake build harness to SNU NAS C sources ="
# make dedicated script visible from the shell
ORIGINAL_PATH=$PATH
export PATH=${ORIGINAL_PATH}:${NAUSEOUS_CMAKE_BUILD_HARNESS_DIR}/utils/scripts/source_tree/

cd ${NAUSEOUS_CMAKE_BUILD_HARNESS_DIR}
create-symlink-bmk-subdir.sh \
    -c ${NAUSEOUS_CMAKE_BUILD_HARNESS_DIR}/config/suite_all.txt \
    -s ${NAS_BENCHMARK_SOURCES_DIR}/NPB3.3-SER-C/ \
    -t ${NAUSEOUS_CMAKE_BUILD_HARNESS_DIR} \
    -l src
cd ${SRC_DIR}

# generate CMake-based testing system
echo "= Generating CMake-based testing system ="

PPAR_METRICS_COLLECTOR_DIR="${BENCHMARKS_BUILD_DIR}/snu-npb-ppar-metrics/"
ICC_LOGS_COLLECTOR_DIR="${BENCHMARKS_BUILD_DIR}/icc-snu-npb-build/"
GCC_COMPILATION_DIR="${BENCHMARKS_BUILD_DIR}/gcc-build/"

if [[ -e ${PPAR_METRICS_COLLECTOR_DIR} ]]; then
    rm -rf ${PPAR_METRICS_COLLECTOR_DIR}
fi
mkdir ${PPAR_METRICS_COLLECTOR_DIR}

if [[ -e ${ICC_LOGS_COLLECTOR_DIR} ]]; then
    rm -rf ${ICC_LOGS_COLLECTOR_DIR}
fi
mkdir ${ICC_LOGS_COLLECTOR_DIR}

if [[ -e ${GCC_COMPILATION_DIR} ]]; then
    rm -rf ${GCC_COMPILATION_DIR}
fi
mkdir ${GCC_COMPILATION_DIR}

echo "= Testing system for SNU NPB PPar metrics collection ="
echo "DIR: ${PPAR_METRICS_COLLECTOR_DIR}"
cd ${PPAR_METRICS_COLLECTOR_DIR} 
build-llvm-ppar-metrics.sh

echo "= Testing system for ICC logs collection ="
echo "DIR: ${ICC_LOGS_COLLECTOR_DIR}"
cd ${ICC_LOGS_COLLECTOR_DIR} 
build-icc-ppar-report.sh

echo "= Testing system for GCC compilation ="
echo "DIR: ${GCC_COMPILATION_DIR}"
cd ${GCC_COMPILATION_DIR} 
build-gcc.sh

export PATH=${ORIGINAL_PATH}
cd ${SRC_DIR}

echo "=== Benchmarks prebuilding script finished! ==="

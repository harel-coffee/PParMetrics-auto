#!/bin/bash

echo "=== Building SPEC CPU2006 benchmarks running directory! ==="

PPAR_PROJECT_DIR="${PWD}"
BENCHMARKS_DIR="${PPAR_PROJECT_DIR}/benchmarks/spec-cpu2006"
SPEC_SOURCES_DIR="${BENCHMARKS_DIR}/spec-cpu2006-raw"
SPECIOUS_HARNESS_DIR="${BENCHMARKS_DIR}/specious"

BENCHMARKS_RUN_DIR="${PPAR_PROJECT_DIR}/benchmarks-run"
SPEC_BENCHMARKS_RUN_DIR="${BENCHMARKS_RUN_DIR}/spec-cpu2006-run"
PPAR_METRICS_RUN_DIR="${SPEC_BENCHMARKS_RUN_DIR}/ppar-metrics-run"
ICC_RUN_DIR="${SPEC_BENCHMARKS_RUN_DIR}/icc-run"

C_FLAGS=""

# check if benchmarks-run directory exists - create if it doesn't
if [[ ! -e ${BENCHMARKS_RUN_DIR} ]]; then
    mkdir ${BENCHMARKS_RUN_DIR}
fi

# clean up all previous builds 
if [[ -e ${SPEC_BENCHMARKS_RUN_DIR} ]]; then
    rm -rf ${SPEC_BENCHMARKS_RUN_DIR}
fi
mkdir ${SPEC_BENCHMARKS_RUN_DIR}

# create symbolic links from the nauseous NAS benchmark build harness to NAS benchmark C source code
echo "= Creating symbolic links from NAS CMake build harness to SNU NAS C sources ="
ORIGINAL_PATH=$PATH

# make dedicated script visible from the shell
export PATH=${ORIGINAL_PATH}:${SPECIOUS_HARNESS_DIR}/utils/scripts/source_tree/
cd ${SPECIOUS_HARNESS_DIR}
create-symlink-bmk-subdir.sh \
    -c ${SPECIOUS_HARNESS_DIR}/configs/all_except_fortran.txt \
    -s ${SPEC_SOURCES_DIR}/CPU2006 \
    -t ${SPECIOUS_HARNESS_DIR} \
    -l src

cd ${PPAR_PROJECT_DIR}

# generate CMake-based testing system
echo "= Generating CMake-based testing system ="

if [[ -e ${PPAR_METRICS_RUN_DIR} ]]; then
    rm -rf ${PPAR_METRICS_RUN_DIR}
fi
mkdir ${PPAR_METRICS_RUN_DIR}

if [[ -e ${ICC_RUN_DIR} ]]; then
    rm -rf ${ICC_RUN_DIR}
fi
mkdir ${ICC_RUN_DIR}

echo "= Testing system for SNU NPB PPar metrics collection ="
echo "DIR: ${PPAR_METRICS_RUN_DIR}"
cd ${PPAR_METRICS_RUN_DIR} 
build-llvm-loopc14n.sh

echo "= Testing system for ICC logs collection ="
echo "DIR: ${ICC_RUN_DIR}"
cd ${ICC_RUN_DIR} 
build-icc-ppar-report.sh

export PATH=${ORIGINAL_PATH}
cd ${PPAR_PROJECT_DIR}

echo "=== Benchmarks prebuilding script finished! ==="

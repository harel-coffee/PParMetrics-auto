#!/bin/bash

echo "=== Building NAS benchmarks running directory! ==="

PPAR_PROJECT_DIR="${PWD}"
BENCHMARKS_DIR="${PPAR_PROJECT_DIR}/benchmarks/snu-npb"
NAS_SOURCES_DIR="${BENCHMARKS_DIR}/SNU_NPB"
NAS_SER_SOURCES_DIR="${NAS_SOURCES_DIR}/NPB3.3-SER-C"
NAS_OMP_SOURCES_DIR="${NAS_SOURCES_DIR}/NPB3.3-OMP-C"
NAUSEOUS_SER_HARNESS_DIR="${BENCHMARKS_DIR}/nauseous"
NAUSEOUS_OMP_HARNESS_DIR="${BENCHMARKS_DIR}/nauseous-omp"
BENCHMARKS_RUN_DIR="${PPAR_PROJECT_DIR}/benchmarks-run"
NAS_BENCHMARKS_RUN_DIR="${BENCHMARKS_RUN_DIR}/snu-npb-run"
PPAR_METRICS_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/ppar-metrics-run"
ICC_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/icc-run"

C_FLAGS=""

if [[ ! -e ${BENCHMARKS_RUN_DIR} ]]; then
    mkdir ${BENCHMARKS_RUN_DIR}
fi

# clean up all previous builds 
if [[ -e ${NAS_BENCHMARKS_RUN_DIR} ]]; then
    rm -rf ${NAS_BENCHMARKS_RUN_DIR}
fi
mkdir ${NAS_BENCHMARKS_RUN_DIR}

# create symbolic links from the nauseous NAS benchmark build harness to NAS benchmark C source code
echo "= Creating symbolic links from NAS CMake build harness to SNU NAS C sources ="
ORIGINAL_PATH=$PATH

# SERIAL NAS BENCHMARKS
# make dedicated script visible from the shell
export PATH=${ORIGINAL_PATH}:${NAUSEOUS_SER_HARNESS_DIR}/utils/scripts/source_tree/
cd ${NAUSEOUS_SER_HARNESS_DIR}
create-symlink-bmk-subdir.sh \
    -c ${NAUSEOUS_SER_HARNESS_DIR}/config/suite_all.txt \
    -s ${NAS_SER_SOURCES_DIR} \
    -t ${NAUSEOUS_SER_HARNESS_DIR} \
    -l src
# PARALLEL NAS BENCHMARKS
export PATH=${ORIGINAL_PATH}:${NAUSEOUS_OMP_HARNESS_DIR}/utils/scripts/source_tree/
cd ${NAUSEOUS_OMP_HARNESS_DIR}
create-symlink-bmk-subdir.sh \
    -c ${NAUSEOUS_OMP_HARNESS_DIR}/config/suite_all.txt \
    -s ${NAS_OMP_SOURCES_DIR} \
    -t ${NAUSEOUS_OMP_HARNESS_DIR} \
    -l src

cd ${PPAR_PROJECT_DIR}

# generate CMake-based testing system
echo "= Generating CMake-based testing system ="
# use OMP version for correct [source_file:line] loop labels
export PATH=${ORIGINAL_PATH}:${NAUSEOUS_OMP_HARNESS_DIR}/utils/scripts/source_tree/

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
build-llvm-ppar-metrics.sh

echo "= Testing system for ICC logs collection ="
echo "DIR: ${ICC_RUN_DIR}"
cd ${ICC_RUN_DIR} 
build-icc-ppar-report.sh

export PATH=${ORIGINAL_PATH}
cd ${PPAR_PROJECT_DIR}

echo "=== Benchmarks prebuilding script finished! ==="

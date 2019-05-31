#!/bin/bash

# < project-environment.sh >
#
# This script is to be included into all project supporting scripts;
# This script sets all the necessary project environment, shell and function 
# variables for the rest of scripts in the set
#

error_exit() {
    echo "$1"
    exit 1
}

STATUS_MSG=""

# [1] check that script is being launched from the project root directory;
# stop and redirect the user to the right directory otherwise; 
STATUS_MSG="Checking script launch directory"
echo $STATUS_MSG
declare -a PROJECT_ROOT_SIGNS=("CMakeLists.txt" # PPar tool buildsystem generation CMake rules 
                               "README.md" # GitHub's README file 
                               ".gitmodules") # Git repository stuff
ERROR_MSG_BASE="Error: the script cannot be launched from this directory! Use the root directory of the project instead."
for file in ${PROJECT_ROOT_SIGNS[@]}; do
    if [ ! -f ${file} ]; then
        ERROR_MSG="${ERROR_MSG_BASE} Could not find a file named ${file}, which is supposed to be present in the root directory."
        error_exit "${ERROR_MSG}"
    fi
done
STATUS_MSG="Script launch directory: OK"
echo $STATUS_MSG

# project directory layout information
PROJECT_ROOT_DIR="${PWD}" # project root directory
# project source directories and files - do not touch and delete automatically
BENCHMARKS_DIR="${PROJECT_ROOT_DIR}/benchmarks"
NAS_BENCHMARKS_DIR="${BENCHMARKS_DIR}/snu-npb"
SPEC_CPU2006_BENCHMARKS_DIR="${BENCHMARKS_DIR}/spec-cpu2006"
NAS_SOURCES_DIR="${NAS_BENCHMARKS_DIR}/SNU_NPB"
NAS_SER_SOURCES_DIR="${NAS_SOURCES_DIR}/NPB3.3-SER-C"
NAS_OMP_SOURCES_DIR="${NAS_SOURCES_DIR}/NPB3.3-OMP-C"
NAUSEOUS_SER_HARNESS_DIR="${NAS_BENCHMARKS_DIR}/nauseous"
NAUSEOUS_OMP_HARNESS_DIR="${NAS_BENCHMARKS_DIR}/nauseous-omp"
PPAR_TOOL_SOURCES_SRC_DIR="${PROJECT_ROOT_DIR}/src"
PPAR_TOOL_SOURCES_INCLUDE_DIR="${PROJECT_ROOT_DIR}/include"
SCRIPTS_DIR="${PROJECT_ROOT_DIR}/scripts"
DOCUMENTS_DIR="${PROJECT_ROOT_DIR}/doc"

# [2] perform a quick repository integrity check;
# stop and complain if some necessary repository pieces are missing; 
STATUS_MSG="Checking repository integrity"
echo $STATUS_MSG
declare -a PROJECT_SOURCE_DIRS=("${BENCHMARKS_DIR}"
                                "${NAS_BENCHMARKS_DIR}"
                                "${SPEC_CPU2006_BENCHMARKS_DIR}"
                                "${NAS_SOURCES_DIR}"
                                "${NAS_SER_SOURCES_DIR}"
                                "${NAS_OMP_SOURCES_DIR}"
                                "${NAUSEOUS_SER_HARNESS_DIR}"
                                "${NAUSEOUS_OMP_HARNESS_DIR}"
                                "${PPAR_TOOL_SOURCES_SRC_DIR}"
                                "${PPAR_TOOL_SOURCES_INCLUDE_DIR}"
                                "${SCRIPTS_DIR}"
                                "${DOCUMENTS_DIR}")
ERROR_MSG_BASE="Error: the script cannot be launched from an incomplete repository!"
for dir in ${PROJECT_SOURCE_DIRS[@]}; do
    if [ ! -d ${dir} ]; then
        ERROR_MSG="${ERROR_MSG_BASE} Necessary ${dir} PPar project directory is missing!"
        error_exit "${ERROR_MSG}"
    fi
done
STATUS_MSG="Repository integrity: OK"
echo $STATUS_MSG

# project auto-generated build files, results, stuff to be cleaned and reobtained, etc.

# various benchmark run directories
BENCHMARKS_RUN_DIR="${PROJECT_ROOT_DIR}/benchmarks-run"
NAS_BENCHMARKS_RUN_DIR="${BENCHMARKS_RUN_DIR}/snu-npb-run"
NAS_PPAR_METRICS_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/ppar-metrics-run"
NAS_ICC_REPORT_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/icc-report-run"
NAS_ICC_PAR_REPORT_RUN_DIR="${NAS_ICC_REPORT_RUN_DIR}/icc-par-report-run"
NAS_ICC_PROF_REPORT_RUN_DIR="${NAS_ICC_REPORT_RUN_DIR}/icc-prof-report-run"
NAS_ICC_PERF_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/icc-perf-run"
NAS_ICC_PERF_SER_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-ser"
NAS_ICC_PERF_VEC_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-vec"
NAS_ICC_PERF_PAR_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-par"
NAS_ICC_PERF_VEC_PAR_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-vec-par"
NAS_ICC_PERF_VEC_OMP_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-vec-omp"
NAS_ICC_PERF_PAR_OMP_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-par-omp"
NAS_ICC_PERF_OMP_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-omp"
NAS_ICC_PERF_VEC_PAR_OMP_DIR="${NAS_ICC_PERF_RUN_DIR}/icc-perf-vec-par-omp"
# order matters: from root directory down to innermost ones
declare -a BENCHMARKS_RUN_DIRS=("${BENCHMARKS_RUN_DIR}" "${NAS_BENCHMARKS_RUN_DIR}" "${NAS_PPAR_METRICS_RUN_DIR}" "${NAS_ICC_REPORT_RUN_DIR}" "${NAS_ICC_PERF_RUN_DIR}" "${NAS_ICC_PERF_SER_DIR}" "${NAS_ICC_PERF_VEC_DIR}" "${NAS_ICC_PERF_PAR_DIR}" "${NAS_ICC_PERF_VEC_PAR_DIR}" "${NAS_ICC_PERF_VEC_OMP_DIR}"  "${NAS_ICC_PERF_PAR_OMP_DIR}"  "${NAS_ICC_PERF_OMP_DIR}" "${NAS_ICC_PERF_VEC_PAR_OMP_DIR}")

PPAR_TOOL_BUILD_DIR="${PROJECT_ROOT_DIR}/build"

REPORTS_DIR="${PROJECT_ROOT_DIR}/reports"
NAS_REPORTS_DIR="${REPORTS_DIR}/snu-npb"

NAS_ICC_REPORTS_DIR="${NAS_REPORTS_DIR}/icc-report"
NAS_ICC_PERF_REPORTS_DIR="${NAS_REPORTS_DIR}/icc-perf-report"
NAS_OMP_GREP_REPORTS_DIR="${NAS_REPORTS_DIR}/omp-grep-report"
NAS_METRICS_REPORTS_DIR="${NAS_REPORTS_DIR}/metrics-report"
NAS_ANALYSIS_REPORTS_DIR="${NAS_REPORTS_DIR}/analysis"
declare -a NAS_REPORTS_DIRS=("${NAS_ICC_REPORTS_DIR}"
                             "${NAS_ICC_PERF_REPORTS_DIR}"
                             "${NAS_OMP_GREP_REPORTS_DIR}"
                             "${NAS_METRICS_REPORTS_DIR}"
                             "${NAS_ANALYSIS_REPORTS_DIR}")

source ${PROJECT_ROOT_DIR}/scripts/include/nas-benchmarks-info.sh

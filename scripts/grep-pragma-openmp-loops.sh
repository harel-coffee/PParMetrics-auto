#!/bin/bash

PPAR_PROJECT_DIR="${PWD}"
BENCHMARKS_DIR="${PPAR_PROJECT_DIR}/benchmarks/"
NAS_SOURCES_DIR="${BENCHMARKS_DIR}/SNU_NPB/"
NAS_OMP_SOURCES_DIR="${NAS_SOURCES_DIR}/NPB3.3-OMP-C/"

REPORT_DIR="${PPAR_PROJECT_DIR}/reports/ppar-metrics/"
GREP_OMP_REPORT_DIR=${REPORT_DIR}/grep-omp-report/

if [[ -e ${GREP_OMP_REPORT_DIR} ]]; then
    rm -rf ${GREP_OMP_REPORT_DIR}
fi
mkdir ${GREP_OMP_REPORT_DIR}

grep "#pragma omp parallel for" ${NAS_SOURCES_DIR} -R -n > ${GREP_OMP_REPORT_DIR}/REPORT.omp
grep "#pragma omp for" ${NAS_SOURCES_DIR} -R -n >> ${GREP_OMP_REPORT_DIR}/REPORT.omp

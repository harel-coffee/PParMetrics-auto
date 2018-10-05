#!/bin/bash

PPAR_PROJECT_DIR="${PWD}"
BENCHMARKS_DIR="${PPAR_PROJECT_DIR}/benchmarks/"
NAS_SOURCES_DIR="${BENCHMARKS_DIR}/nauseous-omp/"
NAS_OMP_SOURCES_DIR="${NAS_SOURCES_DIR}"

REPORT_DIR="${PPAR_PROJECT_DIR}/reports/ppar-metrics/"
GREP_OMP_REPORT_DIR=${REPORT_DIR}/grep-omp-report/

if [[ -e ${GREP_OMP_REPORT_DIR} ]]; then
    rm -rf ${GREP_OMP_REPORT_DIR}
fi
mkdir ${GREP_OMP_REPORT_DIR}

# extract all C/C++ loop related OpenMP pragmas out of source code files
grep "#pragma omp parallel for" ${NAS_OMP_SOURCES_DIR} -R -n > ${GREP_OMP_REPORT_DIR}/REPORT.omp.raw
grep "#pragma omp for" ${NAS_SOURCES_DIR} -R -n >> ${GREP_OMP_REPORT_DIR}/REPORT.omp.raw

cat ${GREP_OMP_REPORT_DIR}/REPORT.omp.raw | 
sed -r 's/\/\//\//g' |
sed -r 's/(.+):(.+):(.+)\\/\1(\2):?/' |
sed -r 's/(.+):([0-9]+):([^?]+)/echo "\1($((\2+1))):1"/ge' \
    > ${GREP_OMP_REPORT_DIR}/REPORT.omp

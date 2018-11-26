#!/bin/bash

source $(dirname "$0")/include/project-environment.sh

echo "< grep-pragma-openmp-loops.sh script >"
echo "=> Grepping #pragma omp out of NAS benchmark sources"

if [[ -e ${NAS_OMP_GREP_REPORTS_DIR} ]]; then
    rm -rf ${NAS_OMP_GREP_REPORTS_DIR}
fi
mkdir ${NAS_OMP_GREP_REPORTS_DIR}

# extract all C/C++ loop related OpenMP pragmas out of source code files
grep "#pragma omp parallel for" ${NAS_OMP_SOURCES_DIR} -R -n > ${NAS_OMP_GREP_REPORTS_DIR}/REPORT.omp.raw
grep "#pragma omp for" ${NAS_OMP_SOURCES_DIR} -R -n >> ${NAS_OMP_GREP_REPORTS_DIR}/REPORT.omp.raw

cat ${NAS_OMP_GREP_REPORTS_DIR}/REPORT.omp.raw | 
sed -r 's/\/\//\//g' |
sed -r 's/(.+):(.+):(.+)\\/\1(\2):?/' |
sed -r 's/(.+):([0-9]+):([^?]+)/echo "\1($((\2+1))):1"/ge' \
    > ${NAS_OMP_GREP_REPORTS_DIR}/REPORT.omp

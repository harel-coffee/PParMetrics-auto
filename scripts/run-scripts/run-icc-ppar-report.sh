#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

echo "< run-icc-ppar-report.sh script >"
echo "=> Running Intel C/C++ compiler on NAS benchmarks in order to collect ICC optimization reports"

if [[ ! -e ${NAS_ICC_RUN_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_RUN_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_REPORTS_DIR} ]]; then
    mkdir ${NAS_REPORTS_DIR}
fi

if [[ -e ${NAS_ICC_REPORTS_DIR} ]]; then
    rm -rf ${NAS_ICC_REPORTS_DIR}
fi
mkdir ${NAS_ICC_REPORTS_DIR}

# compiling NAS benchmarks with Intel C/C++ compiler and generating ICC optimization reports
(
echo "=> Running ICC compiler on the collection of NAS benchmarks"
cd ${NAS_ICC_RUN_DIR}

echo "=> Cleaning NAS benchmarks ICC running directory of previous optimization report files"
make clean
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    echo "Removing ${BenchmarkFolderName}/report.txt"
    rm -rf ${BenchmarkFolderName}/report.txt
done
echo "Removing ${NAS_ICC_RUN_DIR}/REPORT.txt"
rm -rf ${NAS_ICC_RUN_DIR}/REPORT.txt

echo "=> Running ICC compiler on NAS benchmarks with set optimization report generation option"
for BenchmarkName in ${NAS_BENCHMARK_LOWER_CASE_NAMES[@]}; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

echo "=> Moving generated reports into ${NAS_ICC_REPORTS_DIR} Running ICC compiler on NAS benchmarks with set optimization report generation option"
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${NAS_ICC_REPORTS_DIR}/${BenchmarkFolderName}.icc
    cat ${NAS_ICC_RUN_DIR}/${BenchmarkFolderName}/report.txt >> ${NAS_ICC_REPORTS_DIR}/${BenchmarkFolderName}.icc
    echo "${BenchmarkFolderName} benchmark report: ${NAS_ICC_REPORTS_DIR}/${BenchmarkFolderName}.icc"
    cat ${NAS_ICC_REPORTS_DIR}/${BenchmarkFolderName}.icc >> ${NAS_ICC_REPORTS_DIR}/REPORT.icc
done
echo "Final report: ${NAS_ICC_REPORTS_DIR}/REPORT.icc"

# Intel compiler optimization report transformation
python3 ${PROJECT_ROOT_DIR}/scripts/transform-icc-report.py ${NAS_ICC_REPORTS_DIR}/REPORT.icc > ${NAS_ICC_REPORTS_DIR}/REPORT.icc.excel
)

echo "=> NAS benchmarks ICC running script finished!"

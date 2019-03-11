#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

echo "< run-icc-ppar-report.sh script >"
echo "=> Running Intel C/C++ compiler on NAS benchmarks in order to collect ICC profiling reports"

if [[ ! -e ${NAS_ICC_PROF_REPORT_RUN_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_PROF_REPORT_RUN_DIR} has not been found! Run ICC on NAS benchmarks building script first."
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
cd ${NAS_ICC_PROF_REPORT_RUN_DIR}

echo "=> Cleaning NAS benchmarks ICC running directory of previous profiling report files"
make clean
echo "Removing ./loop_prof_*"
rm -rf ./loop_prof_*
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    echo "Removing ${BenchmarkFolderName}/loop_prof_*"
    rm -rf ${BenchmarkFolderName}/loop_prof_*
done

echo "=> Running ICC compiler on NAS benchmarks with set optimization report generation option"
for BenchmarkName in ${NAS_BENCHMARK_LOWER_CASE_NAMES[@]}; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

echo "=> Moving generated reports into ${NAS_ICC_REPORTS_DIR} Running ICC compiler on NAS benchmarks with set optimization report generation option"
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    BENCHMARK_UPPER=${BenchmarkFolderName}
    BENCHMARK_LOWER=$(echo "$BENCHMARK_UPPER" | tr '[:upper:]' '[:lower:]')
    echo "Running Benchmark: ${BenchmarkFolderName}"
    ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}
    cat ./${BENCHMARK_UPPER}/loop_prof_loops_* >> ./loops_profile.report
done
cp ./loops_profile.report ${NAS_ICC_REPORTS_DIR}/loops_profile.report
)

echo "=> NAS benchmarks ICC profile running script finished!"

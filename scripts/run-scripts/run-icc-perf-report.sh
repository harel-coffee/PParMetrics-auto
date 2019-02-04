#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

echo "< run-icc-ppar-report.sh script >"
echo "=> Running Intel C/C++ compiler on NAS benchmarks in order to produce collect ICC optimization reports"

# first we need to set VTune amplifier variables
source /opt/intel/vtune_amplifier_2019/amplxe-vars.sh

if [[ ! -e ${NAS_ICC_RUN_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_RUN_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_REPORTS_DIR} ]]; then
    mkdir ${NAS_REPORTS_DIR}
fi

if [[ -e ${NAS_ICC_PERF_REPORTS_DIR} ]]; then
    rm -rf ${NAS_ICC_PERF_REPORTS_DIR}
fi
mkdir ${NAS_ICC_PERF_REPORTS_DIR}

# running serial versions of NAS benchmarks
(
echo "=> Running serial versions of NAS benchmarks"
cd ${NAS_ICC_PERF_SER_DIR}

echo "=> Cleaning serial ICC run directory of previous performance reports"
make clean
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    echo "Removing ${BenchmarkFolderName}/${BenchmarkFolderName}.out"
    rm -rf ${BenchmarkFolderName}/${BenchmarkFolderName}.out
    echo "Removing serial_perf.report"
    rm -rf serial_perf.report
done

echo "=> Building benchmark binaries with Intel C/C++ compiler on NAS benchmarks"
for BenchmarkName in ${NAS_BENCHMARK_LOWER_CASE_NAMES[@]}; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

echo "=> Running benchmark binaries to produce serial performance report"
for ((i=0;i<${#NAS_BENCHMARK_UPPER_CASE_NAMES[@]};++i)); do
    echo "=> Running ${NAS_BENCHMARK_UPPER_CASE_NAMES[i]} benchmark"
    amplxe-cl -collect hotspots ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]} > "./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out" 2>&1
    echo -n "${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}: " >> serial_perf.report
    grep "Time in seconds" ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out | sed -e 's/[^0-9|.]//g' >> serial_perf.report
    grep "Elapsed Time" ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out | sed -e 's/[^0-9|.]//g' >> serial_perf.report
done

cp serial_perf.report ${NAS_ICC_PERF_REPORTS_DIR} 
)

# running OpenMP versions of NAS benchmarks
(
echo "=> Running OpenMP versions of NAS benchmarks"
cd ${NAS_ICC_PERF_OMP_DIR}

echo "=> Cleaning OpenMP ICC run directory of previous performance reports"
make clean
for BenchmarkFolderName in ${NAS_BENCHMARK_UPPER_CASE_NAMES[@]}; do
    echo "Removing ${BenchmarkFolderName}/${BenchmarkFolderName}.out"
    rm -rf ${BenchmarkFolderName}/${BenchmarkFolderName}.out
    echo "Removing omp_perf.report"
    rm -rf omp_perf.report
done

echo "=> Building benchmark binaries with Intel C/C++ compiler on NAS benchmarks"
for BenchmarkName in ${NAS_BENCHMARK_LOWER_CASE_NAMES[@]}; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

echo "=> Running benchmark binaries to produce OpenMP performance report"
for ((i=0;i<${#NAS_BENCHMARK_UPPER_CASE_NAMES[@]};++i)); do
    echo "=> Running ${NAS_BENCHMARK_UPPER_CASE_NAMES[i]} benchmark"
    amplxe-cl -collect hotspots ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]} > "./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out" 2>&1
    echo -n "${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}: " >> omp_perf.report
    grep "Time in seconds" ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out | sed -e 's/[^0-9|.]//g' >> omp_perf.report
    grep "Elapsed Time" ./${NAS_BENCHMARK_UPPER_CASE_NAMES[i]}/${NAS_BENCHMARK_LOWER_CASE_NAMES[i]}.out | sed -e 's/[^0-9|.]//g' >> omp_perf.report
done

cp omp_perf.report ${NAS_ICC_PERF_REPORTS_DIR} 
)

echo "=> NAS benchmarks performance running script finished!"

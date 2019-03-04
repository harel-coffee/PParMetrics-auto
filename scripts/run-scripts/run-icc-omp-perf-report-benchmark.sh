#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

echo "< run-icc-ppar-report.sh script >"
echo "=> Running Intel C/C++ compiler on NAS benchmarks in order to produce collect ICC optimization reports"

# first we need to set VTune amplifier variables
source /opt/intel/vtune_amplifier_2019/amplxe-vars.sh

BENCHMARK_UPPER=$1
BENCHMARK_LOWER=$(echo "$BENCHMARK_UPPER" | tr '[:upper:]' '[:lower:]')

if [[ ! -e ${NAS_ICC_PERF_RUN_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_PERF_RUN_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_ICC_PERF_OMP_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_PERF_OMP_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_ICC_PERF_PAR_OMP_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_PERF_PAR_OMP_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_REPORTS_DIR} ]]; then
    mkdir ${NAS_REPORTS_DIR}
fi

if [[ -e ${NAS_ICC_PERF_REPORTS_DIR} ]]; then
    rm -rf ${NAS_ICC_PERF_REPORTS_DIR}
fi
mkdir ${NAS_ICC_PERF_REPORTS_DIR}

# running openmp versions of NAS benchmarks
(
echo "=> Running openmp versions of NAS benchmarks"
cd ${NAS_ICC_PERF_OMP_DIR}

echo "=> Cleaning openmp ICC run directory of previous performance reports"
make clean
echo "Removing ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out"
rm -rf ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out
echo "Removing openmp.perf.report"
rm -rf openmp.perf.report

echo "=> Building benchmark binaries with Intel C/C++ compiler on NAS benchmarks"
echo "make ${BENCHMARK_LOWER}"
make ${BENCHMARK_LOWER}

echo "=> Running benchmark binaries to produce openmp performance report"
echo "=> Running ${BENCHMARK_UPPER}"
echo -n "${BENCHMARK_UPPER}: " >> openmp.perf.report
for ((j=0;j<3;++j)); do
    echo "=> ${BENCHMARK_UPPER}#${j}"
    amplxe-cl -collect hotspots ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER} > "./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out" 2>&1
    #grep "Time in seconds" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' >> openmp.perf.report
    grep "Elapsed Time" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' | tr -d '\n' >> openmp.perf.report
    echo -n ":" >> openmp.perf.report
done
echo "" >> openmp.perf.report

cp openmp.perf.report ${NAS_ICC_PERF_REPORTS_DIR}
)

# running vector+parallel+openmp versions of NAS benchmarks
(
echo "=> Running vector+parallel+openmp versions of NAS benchmarks"
cd ${NAS_ICC_PERF_VEC_PAR_OMP_DIR}

echo "=> Cleaning vector+parallel+openmp ICC run directory of previous performance reports"
make clean
echo "Removing ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out"
rm -rf ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out
echo "Removing vector_parallel_openmp.perf.report"
rm -rf vector_parallel_openmp.perf.report

echo "=> Building benchmark binaries with Intel C/C++ compiler on NAS benchmarks"
echo "make ${BENCHMARK_LOWER}"
make ${BENCHMARK_LOWER}

echo "=> Running benchmark binaries to produce vector+parallel+openmp performance report"
echo "=> Running ${BENCHMARK_UPPER}"
echo -n "${BENCHMARK_UPPER}: " >> vector_parallel_openmp.perf.report
for ((j=0;j<3;++j)); do
    echo "=> ${BENCHMARK_UPPER}#${j}"
    amplxe-cl -collect hotspots ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER} > "./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out" 2>&1
    #grep "Time in seconds" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' >> vector_parallel_openmp.perf.report
    grep "Elapsed Time" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' | tr -d '\n' >> vector_parallel_openmp.perf.report
    echo -n ":" >> vector_parallel_openmp.perf.report
done
echo "" >> vector_parallel_openmp.perf.report

cp vector_parallel_openmp.perf.report ${NAS_ICC_PERF_REPORTS_DIR} 
)

echo "=> NAS benchmarks performance running script finished!"

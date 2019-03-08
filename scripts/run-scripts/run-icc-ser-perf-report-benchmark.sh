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

if [[ ! -e ${NAS_ICC_PERF_SER_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks ICC run directory ${NAS_ICC_PERF_SER_DIR} has not been found! Run ICC on NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ ! -e ${NAS_REPORTS_DIR} ]]; then
    mkdir ${NAS_REPORTS_DIR}
fi

if [[ -e ${NAS_ICC_PERF_REPORTS_DIR} ]]; then
    rm -rf ${NAS_ICC_PERF_REPORTS_DIR}
fi
mkdir ${NAS_ICC_PERF_REPORTS_DIR}

# running sequential versions of NAS benchmarks
(
echo "=> Running serial versions of NAS benchmarks"
cd ${NAS_ICC_PERF_SER_DIR}

echo "=> Cleaning serial ICC run directory of previous performance reports"
make clean
echo "Removing ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out"
rm -rf ${BENCHMARK_UPPER}/${BENCHMARK_UPPER}.out
echo "Removing serial.perf.report"
rm -rf serial.perf.report

echo "=> Building benchmark binaries with Intel C/C++ compiler on NAS benchmarks"
echo "make ${BENCHMARK_LOWER}"
make ${BENCHMARK_LOWER}

echo "=> Running benchmark binaries to produce serial performance report"
echo "=> Running ${BENCHMARK_UPPER}"
echo -n "${BENCHMARK_UPPER}: " >> serial.perf.report
for ((j=0;j<3;++j)); do
    echo "=> ${BENCHMARK_UPPER}#${j}"
    amplxe-cl -collect hotspots ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER} > "./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out" 2>&1
    #grep "Time in seconds" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' >> serial.perf.report
    grep "Elapsed Time" ./${BENCHMARK_UPPER}/${BENCHMARK_LOWER}.out | sed -e 's/[^0-9|.]//g' | tr -d '\n' >> serial.perf.report
    echo -n ":" >> serial.perf.report
done
echo "" >> serial.perf.report

cp serial.perf.report ${NAS_ICC_PERF_REPORTS_DIR} 
)

echo "=> NAS benchmarks performance running script finished!"

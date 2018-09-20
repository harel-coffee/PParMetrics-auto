#!/bin/bash

echo "=== Running PPar tool on the collection of NAS benchmarks! ==="

TOOL_DIR="${PWD}"
REPORT_DIR="${PWD}/reports/ppar-metrics/"
BENCHMARKS_BUILD_DIR="${TOOL_DIR}/benchmarks-build/"
PPAR_METRICS_COLLECTOR_DIR="${BENCHMARKS_BUILD_DIR}/snu-npb-ppar-metrics/"
METRICS_REPORT_DIR=${REPORT_DIR}/metrics-ppar-report/

cd ${PPAR_METRICS_COLLECTOR_DIR}

echo "= Cleaning up before the start ="

make clean

if [[ -e ${METRICS_REPORT_DIR} ]]; then
    rm -rf ${METRICS_REPORT_DIR}
fi
mkdir ${METRICS_REPORT_DIR}

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel"
    rm -rf ${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel
done
echo "Removing ${PPAR_METRICS_COLLECTOR_DIR}/REPORT.metrics.excel"
rm -rf ${PPAR_METRICS_COLLECTOR_DIR}/REPORT.metrics.excel

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/*.metrics"
    rm -rf ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/*.metrics
    echo "Removing ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/metrics.excel"
    rm -rf ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/metrics.excel
done 

echo "= Running PPar metrics collector on benchmarks ="

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
    make PParMetrics_${BenchmarkName}_metrics
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel
    cat ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/metrics.excel >> ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel"
    cat ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel >> ${METRICS_REPORT_DIR}/REPORT.metrics.excel
done
echo "Final report: ${METRICS_REPORT_DIR}/REPORT.metrics.excel"

cd ${TOOL_DIR}
echo "=== PPar tool finished! ==="

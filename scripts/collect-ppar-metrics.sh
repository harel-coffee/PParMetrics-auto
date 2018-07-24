#!/bin/bash

echo "=== Collecting PPar metrics on SNU C NAS benchmarks! ==="

SRC_DIR="${PWD}"
BENCHMARKS_BUILD_DIR="${SRC_DIR}/benchmarks-build"
PPAR_METRICS_COLLECTOR_DIR="${BENCHMARKS_BUILD_DIR}/snu-npb-ppar-metrics/"
REPORT_DIR="${PWD}"

cd ${PPAR_METRICS_COLLECTOR_DIR}

echo "Cleaning up before the start"
for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel"
    rm -rf ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel
done
echo "Removing ${PPAR_METRICS_COLLECTOR_DIR}/REPORT.metrics.excel"
rm -rf ${PPAR_METRICS_COLLECTOR_DIR}/REPORT.metrics.excel

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel"
done

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    cp ${PPAR_METRICS_COLLECTOR_DIR}/${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel ${REPORT_DIR}/${BenchmarkFolderName}.metrics.excel
done
cp ${PPAR_METRICS_COLLECTOR_DIR}/REPORT.metrics.excel ${REPORT_DIR}/

echo "Final report: ${TOOL_DIR}/REPORT.metrics.excel"
echo "=== PPar tool finished! ==="

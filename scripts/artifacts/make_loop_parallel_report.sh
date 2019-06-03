#!/bin/bash

echo "=== Running loop parallelisability analyzer on the collection of SNU C NAS benchmarks! ==="

TOOL_DIR="${PWD}"
SCRIPT_LAUNCH_DIR="${PWD}"
BENCHMARKS_DIR="${TOOL_DIR}/benchmarks"

echo "PPar to   ol dir: ${TOOL_DIR}"
echo "NAS benchmarks dir: ${BENCHMARKS_DIR}"

cd ${BENCHMARKS_DIR}

echo "Cleaning up before the start"

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BENCHMARKS_DIR}/${BenchmarkFolderName}.metrics.excel"
done
echo "Removing ${BENCHMARKS_DIR}/REPORT.metrics.excel"

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel"
done
echo "Final report: ${TOOL_DIR}/REPORT.metrics.excel"
echo "=== PPar tool finished! ==="

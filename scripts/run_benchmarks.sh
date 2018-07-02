#!/bin/bash

echo "=== Running PPar tool on the collection of NAS benchmarks! ==="

TOOL_DIR="${PWD}"
BENCHMARKS_DIR="${TOOL_DIR}/benchmarks"

echo "PPar tool dir: ${TOOL_DIR}"
echo "NAS benchmarks dir: ${BENCHMARKS_DIR}"

cd ${BENCHMARKS_DIR}

echo "Cleaning up before the start"
make -C ${BENCHMARKS_DIR} clean

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BENCHMARKS_DIR}/${BenchmarkFolderName}.metrics.excel"
    rm -rf ${BENCHMARKS_DIR}/${BenchmarkFolderName}.metrics.excel
done
echo "Removing ${BENCHMARKS_DIR}/REPORT.metrics.excel"
rm -rf ${BENCHMARKS_DIR}/REPORT.metrics.excel

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BENCHMARKS_DIR}/${BenchmarkFolderName}/*.metrics"
    rm -rf ./${BenchmarkFolderName}/*.metrics
    echo "Removing ${BENCHMARKS_DIR}/${BenchmarkFolderName}/metrics.excel"
    rm -rf ./${BenchmarkFolderName}/metrics.excel
done 

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
    make -C ${BENCHMARKS_DIR} PParMetrics_${BenchmarkName}_metrics
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel
    cat ./${BenchmarkFolderName}/metrics.excel >> ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel"
    cat ${TOOL_DIR}/${BenchmarkFolderName}.metrics.excel >> ./REPORT.metrics.excel
done
echo "Final report: ${TOOL_DIR}/REPORT.metrics.excel"
echo "=== PPar tool finished! ==="

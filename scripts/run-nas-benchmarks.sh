#!/bin/bash

echo "=== Running PPar tool on the collection of NAS benchmarks! ==="

PPAR_PROJECT_DIR="${PWD}"
REPORT_DIR="${PPAR_PROJECT_DIR}/reports/ppar-metrics/"
BENCHMARKS_RUN_DIR="${PPAR_PROJECT_DIR}/benchmarks-run"
NAS_BENCHMARKS_RUN_DIR="${BENCHMARKS_RUN_DIR}/snu-npb-run/"
METRICS_RUN_DIR="${NAS_BENCHMARKS_RUN_DIR}/ppar-metrics-run/"
METRICS_REPORT_DIR=${REPORT_DIR}/ppar-metrics-report/

cd ${METRICS_RUN_DIR}

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
echo "Removing ${METRICS_RUN_DIR}/REPORT.metrics.excel"
rm -rf ${METRICS_RUN_DIR}/REPORT.metrics.excel

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${METRICS_RUN_DIR}/${BenchmarkFolderName}/*.metrics"
    rm -rf ${METRICS_RUN_DIR}/${BenchmarkFolderName}/*.metrics
    echo "Removing ${METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel"
    rm -rf ${METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel
done 

echo "= Running PPar metrics collector on benchmarks ="

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
    make PParMetrics_${BenchmarkName}_metrics
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel
    cat ${METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel >> ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel"
    cat ${METRICS_REPORT_DIR}/${BenchmarkFolderName}.metrics.excel >> ${METRICS_REPORT_DIR}/REPORT.metrics.excel
done
echo "Final report: ${METRICS_REPORT_DIR}/REPORT.metrics.excel"

cd ${PPAR_PROJECT_DIR}
echo "=== PPar tool finished! ==="

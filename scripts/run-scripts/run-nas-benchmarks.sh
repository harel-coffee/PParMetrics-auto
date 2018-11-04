#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

echo "< run-nas-ppar-metrics.sh script >"
echo "=> Running PPar tool on NAS benchmarks in order to collect metrics"

if [[ ! -e ${NAS_PPAR_METRICS_RUN_DIR} ]]; then
    ERROR_MSG="Error: NAS benchmarks PPar tool run directory ${NAS_PPAR_METRICS_RUN_DIR} has not been found! Run NAS benchmarks building script first."
    error_exit "${ERROR_MSG}" 
fi

if [[ -e ${NAS_METRICS_REPORTS_DIR} ]]; then
    rm -rf ${NAS_METRICS_REPORTS_DIR}
fi
mkdir ${NAS_METRICS_REPORTS_DIR}

(
cd ${NAS_PPAR_METRICS_RUN_DIR}

make clean
if [[ -e ${NAS_METRICS_REPORTS_DIR} ]]; then
    rm -rf ${NAS_METRICS_REPORTS_DIR}
fi
mkdir ${NAS_METRICS_REPORTS_DIR}

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

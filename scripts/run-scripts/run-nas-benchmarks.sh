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
for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel"
    rm -rf ${BenchmarkFolderName}/${BenchmarkFolderName}.metrics.excel
done
echo "Removing ${NAS_PPAR_METRICS_RUN_DIR}/REPORT.metrics.excel"
rm -rf ${NAS_PPAR_METRICS_RUN_DIR}/REPORT.metrics.excel

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${NAS_PPAR_METRICS_RUN_DIR}/${BenchmarkFolderName}/*.metrics"
    rm -rf ${NAS_PPAR_METRICS_RUN_DIR}/${BenchmarkFolderName}/*.metrics
    echo "Removing ${NAS_PPAR_METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel"
    rm -rf ${NAS_PPAR_METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel
done 

echo "= Running PPar metrics collector on benchmarks ="

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make PParMetrics_${BenchmarkName}_metrics"
    make PParMetrics_${BenchmarkName}_metrics
done 

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${NAS_METRICS_REPORTS_DIR}/${BenchmarkFolderName}.metrics.excel
    cat ${NAS_PPAR_METRICS_RUN_DIR}/${BenchmarkFolderName}/metrics.excel >> ${NAS_METRICS_REPORTS_DIR}/${BenchmarkFolderName}.metrics.excel
    echo "${BenchmarkFolderName} benchmark report: ${NAS_METRICS_REPORTS_DIR}/${BenchmarkFolderName}.metrics.excel"
    cat ${NAS_METRICS_REPORTS_DIR}/${BenchmarkFolderName}.metrics.excel >> ${NAS_METRICS_REPORTS_DIR}/REPORT.metrics.excel
done
echo "Final report: ${NAS_METRICS_REPORTS_DIR}/REPORT.metrics.excel"

)

echo "=== PPar tool finished! ==="

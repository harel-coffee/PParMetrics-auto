#!/bin/bash

echo "=== Running ICC compiler on the collection of NAS benchmarks! ==="

PPAR_PROJECT_DIR="${PWD}"
REPORT_DIR="${PPAR_PROJECT_DIR}/reports/ppar-metrics/"
BENCHMARKS_RUN_DIR="${PPAR_PROJECT_DIR}/benchmarks-run/"
ICC_RUN_DIR="${BENCHMARKS_RUN_DIR}/nas-icc-run/"
ICC_REPORT_DIR=${REPORT_DIR}/icc-par-report/

cd ${ICC_RUN_DIR}

echo "= Cleaning up before the start ="

if [[ -e ${ICC_REPORT_DIR} ]]; then
    rm -rf ${ICC_REPORT_DIR}
fi
mkdir ${ICC_REPORT_DIR}

make clean

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Removing ${BenchmarkFolderName}/report.txt"
    rm -rf ${BenchmarkFolderName}/report.txt
done
echo "Removing ${ICC_RUN_DIR}/REPORT.txt"
rm -rf ${ICC_RUN_DIR}/REPORT.txt

echo "= Running ICC compiler on benchmarks ="

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc
    cat ${ICC_RUN_DIR}/${BenchmarkFolderName}/report.txt >> ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc
    echo "${BenchmarkFolderName} benchmark report: ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc"
    cat ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc >> ${ICC_REPORT_DIR}/REPORT.icc
done
echo "Final report: ${ICC_REPORT_DIR}/REPORT.icc"

python3 ${PPAR_PROJECT_DIR}/scripts/transform-icc-report.py ${ICC_REPORT_DIR}/REPORT.icc > ${ICC_REPORT_DIR}/REPORT.icc.excel

cd ${PPAR_PROJECT_DIR}
echo "=== ICC running script finished! ==="

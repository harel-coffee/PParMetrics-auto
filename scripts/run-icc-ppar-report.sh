#!/bin/bash

echo "=== Running ICC compiler on the collection of NAS benchmarks! ==="

TOOL_DIR="${PWD}"
REPORT_DIR="${PWD}/reports/"
BENCHMARKS_BUILD_DIR="${TOOL_DIR}/benchmarks-build/"
ICC_PPAR_RUN_DIR="${BENCHMARKS_BUILD_DIR}/icc-snu-npb-build/"
ICC_REPORT_DIR=${REPORT_DIR}/icc-ppar-report/

cd ${ICC_PPAR_RUN_DIR}

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
echo "Removing ${ICC_PPAR_RUN_DIR}/REPORT.txt"
rm -rf ${ICC_PPAR_RUN_DIR}/REPORT.txt

echo "= Running ICC compiler on benchmarks ="

for BenchmarkName in bt cg dc ep ft is lu mg sp ua; do
    echo "make ${BenchmarkName}"
    make ${BenchmarkName}
done

for BenchmarkFolderName in BT CG DC EP FT IS LU MG SP UA; do
    echo "Benchmark: ${BenchmarkFolderName}" >> ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc
    cat ${ICC_PPAR_RUN_DIR}/${BenchmarkFolderName}/report.txt >> ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc
    echo "${BenchmarkFolderName} benchmark report: ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc"
    cat ${ICC_REPORT_DIR}/${BenchmarkFolderName}.icc >> ${ICC_REPORT_DIR}/REPORT.icc
done
echo "Final report: ${ICC_REPORT_DIR}/REPORT.icc"

cd ${TOOL_DIR}
echo "=== ICC running script finished! ==="

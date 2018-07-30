#!/bin/bash

echo "=== Running statistical analysis of PPar metrics! ==="

TOOL_DIR="${PWD}"
REPORT_DIR="${PWD}/reports/"
ANALYSIS_DIR="${PWD}/analysis/"
ANALYSIS_REPORT_DIR=${REPORT_DIR}/analysis/

cd ${ICC_PPAR_RUN_DIR}

echo "= Cleaning up before the start ="

if [[ -e ${ANALYSIS_REPORT_DIR} ]]; then
    rm -rf ${ANALYSIS_REPORT_DIR}
fi
mkdir ${ANALYSIS_REPORT_DIR}

RAW_DATA_FILENAME="${ANALYSIS_DIR}/raw_data.csv"

SVM_REPORT_FILENAME="${ANALYSIS_REPORT_DIR}/SVM.report"
python3 ${ANALYSIS_DIR}/svm.py ${RAW_DATA_FILENAME} ${SVM_REPORT_FILENAME}

echo "=== Statistical analysis script finished! ==="

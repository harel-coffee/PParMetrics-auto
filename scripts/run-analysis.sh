#!/bin/bash

echo "=== Running statistical analysis of PPar metrics! ==="

TOOL_DIR="${PWD}"
REPORT_DIR="${PWD}/reports"
ANALYSIS_SCRIPTS_DIR="${PWD}/scripts/analysis"
ANALYSIS_DATA_DIR="${PWD}/analysis"
ANALYSIS_REPORT_DIR=${REPORT_DIR}/analysis

cd ${ICC_PPAR_RUN_DIR}

echo "= Cleaning up before the start ="

if [[ -e ${ANALYSIS_REPORT_DIR} ]]; then
    rm -rf ${ANALYSIS_REPORT_DIR}
fi
mkdir ${ANALYSIS_REPORT_DIR}

RAW_DATA_FILENAME="${ANALYSIS_DATA_DIR}/raw_data.csv"

SVM_REPORT_FILENAME="${ANALYSIS_REPORT_DIR}/SVM.report"
python3 ${ANALYSIS_SCRIPTS_DIR}/svm.py ${RAW_DATA_FILENAME} ${SVM_REPORT_FILENAME} 3 n

echo "=== Statistical analysis script finished! ==="

#!/bin/bash

echo "=== Running visual reports producing script! ==="

TOOL_DIR="${PWD}"
ANALYSIS_DATA_DIR="${TOOL_DIR}/analysis"
REPORT_DIR="${PWD}/reports"
ANALYSIS_SCRIPTS_DIR="${TOOL_DIR}/scripts/analysis"
VISUAL_REPORT_DIR="${REPORT_DIR}/visual"

echo "= Cleaning up ${VISUAL_REPORT_DIR} before the start ="

if [[ -e ${VISUAL_REPORT_DIR} ]]; then
    rm -rf ${VISUAL_REPORT_DIR}
fi
mkdir ${VISUAL_REPORT_DIR}

RAW_DATA="${ANALYSIS_DATA_DIR}/raw_data.csv"

SINGLE_REPORT_DIRECTORY="${VISUAL_REPORT_DIR}/single"
CLUSTER_REPORT_DIRECTORY="${VISUAL_REPORT_DIR}/cluster"

mkdir "${SINGLE_REPORT_DIRECTORY}"

for ((i = 0; i < 4; ++i)); do
    STD_DEV_DIR="${SINGLE_REPORT_DIRECTORY}/std-dev-${i}"
    mkdir ${STD_DEV_DIR}
    python3 ${ANALYSIS_SCRIPTS_DIR}/visualize-single-metric-parallelizability.py "${RAW_DATA}" "${STD_DEV_DIR}" $i
done

python3 ${ANALYSIS_SCRIPTS_DIR}/visualize-features-pca-3d-exp.py "${RAW_DATA}" "${CLUSTER_REPORT_DIRECTORY}" 3

echo "=== Reports visualization script finished! ==="

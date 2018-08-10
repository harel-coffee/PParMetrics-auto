#!/bin/bash

echo "=== Running decision trees visualization script! ==="

TOOL_DIR="${PWD}"
ANALYSIS_DATA_DIR="${TOOL_DIR}/analysis"
REPORT_DIR="${PWD}/reports"
ANALYSIS_SCRIPTS_DIR="${TOOL_DIR}/scripts/analysis"
DECISION_TREE_VIS_DIRECTORY="${REPORT_DIR}/dt-visual"

echo "= Cleaning up ${DECISION_TREE_VIS_DIRECTORY} before the start ="

if [[ -e ${DECISION_TREE_VIS_DIRECTORY} ]]; then
    rm -rf ${DECISION_TREE_VIS_DIRECTORY}
fi
mkdir ${DECISION_TREE_VIS_DIRECTORY}

RAW_DATA="${ANALYSIS_DATA_DIR}/raw_data.csv"

python3 ${ANALYSIS_SCRIPTS_DIR}/visualize-decision-trees.py "${RAW_DATA}" "${DECISION_TREE_VIS_DIRECTORY}" 3 n
find ${DECISION_TREE_VIS_DIRECTORY} -regextype 'egrep' \
    -iregex '.*\.dot' -exec dot -Tpdf {} -o {}.pdf \;

#for D in $(`find ${CISION_TREE_VIS_DIRECTORY} -type d -regextype posix-extended -regex '(.*).dot$' `)
#do
#    echo ${D}
#    echo ${BASH_REMATCH[1]}
#    `dot -Tpdf D  -o ${DECISION_TREE_VIS_DIRECTORY}/${BASH_REMATCH[1]}.pdf` 
#done

echo "=== DT visualization script finished! ==="

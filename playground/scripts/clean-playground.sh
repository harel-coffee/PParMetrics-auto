#!/bin/bash

echo -e "=======\n\
Cleaning up all the generated data:\n \
* LLVM assembly files\n \
* Host platform assembly files\n \
* Executables\n \
* DOT files\n \
* PDF Graphs\n \
* ICC reports\n \
* etc.\n\
======="

PLAYGROUND_DIR="${PWD}"
SOURCES_DIR="${PLAYGROUND_DIR}/src"
ASSEMBLY_DIR="${PLAYGROUND_DIR}/asm"
DOTS_DIR="${PLAYGROUND_DIR}/dot"
PDFS_DIR="${PLAYGROUND_DIR}/pdf"
EXECUTABLES_DIR="${PLAYGROUND_DIR}/exe"
METRICS_DIR="${PLAYGROUND_DIR}/metric"
REPORTS_DIR="${PLAYGROUND_DIR}/report"

declare -a GENERATED_OUTPUT_DIRS=("${ASSEMBLY_DIR}" "${DOTS_DIR}" "${PDFS_DIR}" "${EXECUTABLES_DIR}" "${METRICS_DIR}" "${REPORTS_DIR}")

for dir in ${GENERATED_OUTPUT_DIRS[@]}; do
    if [[ -e ${dir} ]]; then
        rm -rf ${dir}
        echo "Deleted $dir"
    fi
done

echo "=== clean-playground.sh script finished! ==="

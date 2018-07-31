#!/bin/bash

echo "=== Cleaning up all reports, builds, runs! ==="

TOOL_DIR="${PWD}"
BENCHMARKS_BUILD_DIR="${TOOL_DIR}/benchmarks-build/"
TOOL_BUILD_DIR="${TOOL_DIR}/build/"
REPORTS_DIR="${TOOL_DIR}/reports/"
ICC_COMPILER_PPAR_REPORTS_DIR="${REPORTS_DIR}/icc-ppar-report/"
METRICS_PPAR_REPORTS_DIR="${REPORTS_DIR}/metrics-ppar-report/"
ANALYSIS_REPORTS_DIR="${REPORTS_DIR}/analysis/"

if [[ -e ${ICC_COMPILER_PPAR_REPORTS_DIR} ]]; then
    rm -rf ${ICC_COMPILER_PPAR_REPORTS_DIR}
fi

if [[ -e ${METRICS_PPAR_REPORTS_DIR} ]]; then
    rm -rf ${METRICS_PPAR_REPORTS_DIR}
fi

if [[ -e ${ANALYSIS_REPORTS_DIR} ]]; then
    rm -rf ${ANALYSIS_REPORTS_DIR}
fi

if [[ -e ${BENCHMARKS_BUILD_DIR} ]]; then
    rm -rf ${BENCHMARKS_BUILD_DIR}/*
fi

if [[ -e ${TOOL_BUILD_DIR} ]]; then
    rm -rf ${TOOL_BUILD_DIR}/*
fi

echo "=== Purge script finished! ==="

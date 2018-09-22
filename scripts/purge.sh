#!/bin/bash

echo "=== Cleaning up all reports, builds, runs! ==="

PPAR_PROJECT_DIR="${PWD}"
PPAR_TOOL_BUILD_DIR="${PPAR_PROJECT_DIR}/build/"
BENCHMARKS_RUN_DIR="${PPAR_PROJECT_DIR}/benchmarks-run/"
REPORTS_DIR="${PPAR_PROJECT_DIR}/reports/ppar-metrics/"
ICC_REPORTS_DIR="${REPORTS_DIR}/icc-report/"
METRICS_REPORTS_DIR="${REPORTS_DIR}/metrics-report/"
ANALYSIS_REPORTS_DIR="${REPORTS_DIR}/analysis/"

if [[ -e ${ICC_REPORTS_DIR} ]]; then
    rm -rf ${ICC_REPORTS_DIR}
fi

if [[ -e ${METRICS_REPORTS_DIR} ]]; then
    rm -rf ${METRICS_REPORTS_DIR}
fi

if [[ -e ${ANALYSIS_REPORTS_DIR} ]]; then
    rm -rf ${ANALYSIS_REPORTS_DIR}
fi

if [[ -e ${BENCHMARKS_RUN_DIR} ]]; then
    rm -rf ${BENCHMARKS_RUN_DIR}/*
    rm -rf ${BENCHMARKS_RUN_DIR}
fi

if [[ -e ${PPAR_TOOL_BUILD_DIR} ]]; then
    rm -rf ${PPAR_TOOL_BUILD_DIR}/*
    rm -rf ${PPAR_TOOL_BUILD_DIR}
fi

echo "=== Purge script finished! ==="

#!/bin/bash

source $(dirname "$0")/../include/project-environment.sh

ML_MODEL=$1

for BenchmarkName in ${NAS_BENCHMARK_LOWER_CASE_NAMES[@]}; do
    echo "${BenchmarkName}"
    rm -rf ./ml_train_test/train-test-dyn-omp/${ML_MODEL}/${BenchmarkName}/*
    python3 ./scripts/analysis/tt_ml_pipeline.py ./ml_train_test/train-test-dyn-data-omp/${BenchmarkName}.train.csv ./ml_train_test/train-test-dyn-data-omp/${BenchmarkName}.test.csv ./scripts/analysis/config/${ML_MODEL}.cfg.ini ./ml_train_test/train-test-dyn-omp/${ML_MODEL}/${BenchmarkName}/
done


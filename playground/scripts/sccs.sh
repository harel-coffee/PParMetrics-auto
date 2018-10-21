#!/bin/bash

clang++ -S -emit-llvm ./sources/$1.cpp -o $1.ll
clang++ ./sources/$1.cpp -o $1
mv $1 execs/$1

opt -O3 $1.ll -o $1.bc
llvm-dis $1.bc -o $1.ll
mv $1.ll ./assembly/$1/$1.ll
rm -rf $1.bc
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -sccs ./assembly/$1/$1.ll

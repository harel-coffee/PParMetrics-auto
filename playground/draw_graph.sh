#!/bin/bash

clang++ -S -emit-llvm ./sources/$1.cpp -o $1.ll
clang++ ./sources/$1.cpp -o $1
mv $1 execs
opt -O3 $1.ll -o $1.bc
llvm-dis $1.bc -o $1.ll
mv $1.ll ./assembly/$1.ll
rm -rf $1.bc
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -dot-pdg ./assembly/$1.ll
mkdir ./dots/$1
mkdir ./pdfs/$1
mv *.dot ./dots/$1
cd ./dots/$1
for f in *
do
    echo $f 
    dot -Tpdf $f -o $f.pdf
done
mv *.pdf ../../pdfs/$1
cd ..
cd ..

#!/bin/bash

if [ -d "./dots/$1" ]; then
    rm -rf ./dots/$1
    mkdir ./dots/$1
else
    mkdir ./dots/$1
fi

if [ -d "./pdfs/$1" ]; then
    rm -rf ./pdfs/$1
    mkdir ./pdfs/$1
else
    mkdir ./pdfs/$1
fi

if [ -d "./execs/$1" ]; then
    rm -rf ./execs/$1
    mkdir ./execs/$1
else
    mkdir ./execs/$1
fi

if [ -d "./assembly/$1" ]; then
    rm -rf ./assembly/$1
    mkdir ./assembly/$1
else
    mkdir ./assembly/$1
fi

clang++ -S -emit-llvm ./sources/$1.cpp -o $1.ll
clang++ ./sources/$1.cpp -o $1
mv $1 execs/$1

opt -O3 $1.ll -o $1.bc
llvm-dis $1.bc -o $1.ll
mv $1.ll ./assembly/$1/$1.ll
rm -rf $1.bc
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -dot-ddg ./assembly/$1/$1.ll
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -dot-mdg ./assembly/$1/$1.ll
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -dot-cdg ./assembly/$1/$1.ll
opt -load /home/s1736883/Work/PParMetrics-build/libppar.so -dot-pdg ./assembly/$1/$1.ll
mv *.dot ./dots/$1
cd ./dots/$1
for f in *
do
    echo $f 
    dot -Tpdf $f -o $f.pdf
done
mv *.pdf ../../pdfs/$1
cd ../..

#!/bin/bash

LLVM_BIN_DIR=$1

if [ $LLVM_BIN_DIR != "" ]
then
    export PATH=$LLVM_BIN_DIR:$PATH
    echo "LLVM bin directory is:"
    echo $LLVM_BIN_DIR
    echo "Set PATH is:"
    echo $PATH
else
    echo "Usage: ./init_playground.sh LLVM_BIN_DIR_PATH"
fi

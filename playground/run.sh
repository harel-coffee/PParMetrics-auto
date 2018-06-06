#!/bin/bash

for f in sources/*
do
    FILENAME=${f#*/}
    FILENAME=${FILENAME%.*}
    echo $FILENAME
    ./do.sh $FILENAME
done

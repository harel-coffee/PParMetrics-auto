#!/bin/bash

PLAYGROUND_DIR="${PWD}"
SOURCES_DIR="${PLAYGROUND_DIR}/sources"
SCRIPTS_DIR="${PLAYGROUND_DIR}/scripts"

SOURCES=""
for src in ${SOURCES_DIR}/*
do
    FILENAME=${src#*/}
    FILENAME=${FILENAME%.*}
    echo $FILENAME
    SOURCES="${SOURCES} ${FILENAME}"
done

. ${SCRIPTS_DIR}/do.sh ${SOURCES}

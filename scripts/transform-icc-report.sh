#!/bin/bash

report_loop()
{
    DEPTH=$((DEPTH+1))
    local LOOP_DEPTH=${DEPTH}
    local LOOP_FILE=$1
    local LOOP_LINE=$2

    # 10 stands for uninitialized
    local REPORT_LOOP=10
    local PARALLELIZED=10
    while ! [[ ${line} =~ LOOP[[:space:]]END ]];
    do 
        read line 
        
        if [[ ${line} =~ LOOP[[:space:]]BEGIN[[:space:]]at[[:space:]]([^(|)]*)(.*) ]]; then
            local FILE=${BASH_REMATCH[1]}
            local LINE=${BASH_REMATCH[2]%,*}
            LINE=${LINE:1}

            report_loop ${FILE} ${LINE}
        elif [[ ${line} =~ LOOP[[:space:]]WAS[[:space:]]AUTO-PARALLELIZED ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=1
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=1
            fi
        elif [[ ${line} =~ No[[:space:]]loop[[:space:]]optimizations[[:space:]]reported ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=1
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=0
            fi
        elif [[ ${line} =~ Remainder[[:space:]]loop[[:space:]]for[[:space:]]vectorization ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=0
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=0
            fi
        elif [[ ${line} =~ Remainder ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=0
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=1
            fi
        elif [[ ${line} =~ Peeled[[:space:]]loop[[:space:]]for[[:space:]]vectorization ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=1
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=1
            fi
        elif [[ ${line} =~ LOOP[[:space:]]END ]]; then
            if [[ ${REPORT_LOOP} -eq 10 ]]; then
                REPORT_LOOP=1
            fi
            if [[ ${PARALLELIZED} -eq 10 ]]; then
                PARALLELIZED=2
            fi
        elif [[ ${line} =~ Distributed[[:space:]]chunk ]]; then
            :
        fi
    done

    # loop reporting to the output
    if [[ ${REPORT_LOOP} -eq 1 ]]; then
        if [[ ${PARALLELIZED} -eq 2 ]]; then
            echo "${LOOP_FILE}(${LOOP_LINE}):${LOOP_DEPTH}:?"
        elif [[ ${PARALLELIZED} -eq 1 ]]; then
            echo "${LOOP_FILE}(${LOOP_LINE}):${LOOP_DEPTH}:1"
        elif [[ ${PARALLELIZED} -eq 0 ]]; then
            echo "${LOOP_FILE}(${LOOP_LINE}):${LOOP_DEPTH}:0"
        fi
    fi

    line=""

    DEPTH=$((DEPTH-1))
}

DEPTH=0 # global variable - current loop depth
while read line
do
    if [[ ${line} =~ LOOP[[:space:]]BEGIN[[:space:]]at[[:space:]]([^(|)]*)(.*) ]]; then
        # identified outer loop
        FILE=${BASH_REMATCH[1]}
        LINE=${BASH_REMATCH[2]%,*}
        LINE=${LINE:1}
        
        report_loop ${FILE} ${LINE}
    elif [[ ${line} =~ Benchmark:[[:space:]](.*) ]]; then
        echo "${BASH_REMATCH[1]}"
    fi
done

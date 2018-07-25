#!/bin/bash

report_loop()
{
    DEPTH=$((DEPTH+1))
    LOOP_FILE=$1
    LOOP_LINE=$2
   
    while ! [[ ${line} =~ LOOP[[:space:]]END ]];
    do 
        read line 
        
        if [[ ${line} =~ LOOP[[:space:]]BEGIN[[:space:]]at[[:space:]]([^(|)]*)(.*) ]]; then
            FILE=${BASH_REMATCH[1]}
            LINE=${BASH_REMATCH[2]%,*}
            LINE=${LINE:1}

            report_loop FILE LINE
        elif [[ ${line} =~ LOOP[[:space:]]WAS[[:space:]]AUTO-PARALLELIZED ]]; then
            echo "${FILE}(${LINE}):${DEPTH}:1"
        elif [[ ${line} =~ No[[:space:]]loop[[:space:]]optimizations[[:space:]]reported ]]; then
            echo "${FILE}(${LINE}):${DEPTH}:0"
        elif [[ ${line} =~ Remainder[[:space:]]loop[[:space:]]for[[:space:]]vectorization ]]; then
            :
        elif [[ ${line} =~ Peeled[[:space:]]loop[[:space:]]for[[:space:]]vectorization ]]; then
            echo "${FILE}(${LINE}):${DEPTH}:1"
        elif [[ ${line} =~ LOOP[[:space:]]END ]]; then
            echo "${FILE}(${LINE}):${DEPTH}:?"
        elif [[ ${line} =~ Distributed[[:space:]]chunk1 ]]; then
            read line 
            if [[ ${line} =~ DISTRIBUTED[[:space:]]LOOP[[:space:]]WAS[[:space:]]AUTO-PARALLELIZED ]]; then
                echo "${FILE}(${LINE}):${DEPTH}:1"
            fi 
        elif [[ ${line} =~ Distributed[[:space:]]chunk2 ]]; then
            : 
        fi
    done

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
        
        report_loop FILE LINE
    fi
done

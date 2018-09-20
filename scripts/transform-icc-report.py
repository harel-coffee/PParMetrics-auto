#! /usr/bin/python3

import re
import sys

LOOP_BEGIN_RE = re.compile("LOOP BEGIN at (.*)\((.*),")
LOOP_END_RE = re.compile("LOOP END")
LOOP_PARALLEL_RE = re.compile("LOOP WAS AUTO-PARALLELIZED")
LOOP_REMAINDER_0_RE = re.compile("Remainder loop for vectorization")
LOOP_REMAINDER_1_RE = re.compile("Remainder")
LOOP_PEELED_RE = re.compile("Peeled loop for vectorization")
LOOP_NON_PARALLEL_RE = re.compile("loop was not parallelized")
LOOP_NO_OPTIMIZATIONS_RE = re.compile("No loop optimizations reported")

def process_loop(loop_name):
    global loop_depth
    global icc_report_file
    global loop_classification
    loop_depth += 1
 
    while True:
        line = icc_report_file.readline()

        # detected inner loop
        loop_begin_match = LOOP_BEGIN_RE.search(line)
        if loop_begin_match != None:
            subloop_name = loop_begin_match.group(1) + "(" + loop_begin_match.group(2) + ")"
            if subloop_name not in loop_classification:
                loop_classification[subloop_name] = 3 # uninitialized - haven't seen loop remarks yet
            process_loop(subloop_name)
            continue
        
        # loop is parallelizible
        loop_parallel_match = LOOP_PARALLEL_RE.search(line)
        if loop_parallel_match != None:
            if loop_classification[loop_name] == 3 or loop_classification[loop_name] == 0:
                loop_classification[loop_name] = 1 # parallelizible
            continue

        # loop remainder, split, peel etc. - do not carry classifying information
        loop_remainder_0_match = LOOP_REMAINDER_0_RE.search(line)
        loop_remainder_1_match = LOOP_REMAINDER_1_RE.search(line)
        if loop_remainder_0_match != None or loop_remainder_1_match != None:
            continue

        # loop is not parallelizible
        loop_non_parallel_match = LOOP_NON_PARALLEL_RE.search(line)
        if loop_non_parallel_match != None:
            if loop_classification[loop_name] == 3:
                loop_classification[loop_name] = 0 # non-parallelizible
            continue

        # peeled loop for vectorization
        loop_peeled_match = LOOP_PEELED_RE.search(line)
        if loop_peeled_match != None:
            if loop_classification[loop_name] == 3 or loop_classification[loop_name] == 0:
                loop_classification[loop_name] = 1 # peeled -> parallelized
            continue

        # no loop optimizations
        loop_no_optimizations_match = LOOP_NO_OPTIMIZATIONS_RE.search(line)
        if loop_no_optimizations_match != None:
            if loop_classification[loop_name] == 3:
                loop_classification[loop_name] = 0 # non-parallelizible
            continue

        # the end of the loop
        loop_end_match = LOOP_END_RE.search(line)
        if loop_end_match != None:
            break

    loop_depth -= 1
    return

if __name__ == "__main__":

    # 0 - non-parallel, 1 - parallel, 2 - don't know, 3 - uninitialized
    #
    loop_classification = {}

    loop_depth = 0
    icc_report_file = open(sys.argv[1],"r")
    while True:
        line = icc_report_file.readline()
        if line == "":
            break
        loop_begin_match = LOOP_BEGIN_RE.search(line)
        if loop_begin_match != None:
            loop_name = loop_begin_match.group(1) + "(" + loop_begin_match.group(2) + ")"
            if loop_name not in loop_classification:
                loop_classification[loop_name] = 3 # uninitialized - haven't seen loop remarks yet
            process_loop(loop_name)
    
    for name, classification in loop_classification.items():
        if (classification == 0) or (classification == 1):
            print(name + ":" + str(classification))
        elif (classification == 3):
            print(name + ":" + "?")

    print("Done!")

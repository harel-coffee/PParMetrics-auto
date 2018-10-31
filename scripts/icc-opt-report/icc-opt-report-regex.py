#! /usr/bin/python3

import re

# [1] Regular expressions identifying loop boundaries in ICC compiler report
LOOP_BEGIN_RE = re.compile("LOOP BEGIN at (.+)\(([0-9]+),.+")
LOOP_END_RE = re.compile("LOOP END")
LOOP_NAME_RE = re.compile("(.+)\(([0-9]+)\)")

# [2] Regular expressions for different sorts of loop remarks, 
# provided by the Intel compiler in it's report

# [2.1] ICC compiler OpenMP report
OPENMP_CONSTRUCT_RE = re.compile("OpenMP Construct at (.*)\((.*),.+")
OPENMP_PARALLEL_RE = re.compile("OpenMP DEFINED LOOP PARALLELIZED")

# [2.2] remarks identifying parallelized loops 
LOOP_PARALLELIZED_RE = re.compile(": LOOP WAS AUTO-PARALLELIZED")
LOOP_DISTR_PARALLELIZED_RE = re.compile(": DISTRIBUTED LOOP WAS AUTO-PARALLELIZED")
LOOP_FUSED_PARALLELIZED_RE = re.compile(": FUSED LOOP WAS AUTO-PARALLELIZED")
LOOP_PARTIAL_PARALLELIZED_RE = re.compile(": PARTIAL LOOP WAS AUTO-PARALLELIZED")
LOOP_INNER_PAR_POTENTIAL_RE = re.compile("loop was not parallelized: inner loop")

# [2.3] remarks identifying vectorized loops 
LOOP_VECTORIZED_RE = re.compile(": LOOP WAS VECTORIZED")
LOOP_DISTR_VECTORIZED_RE = re.compile(": DISTRIBUTED LOOP WAS VECTORIZED")
LOOP_FUSED_VECTORIZED_RE = re.compile(": FUSED LOOP WAS VECTORIZED")
LOOP_PARTIAL_VECTORIZED_RE = re.compile(": PARTIAL LOOP WAS VECTORIZED")
LOOP_INNER_VEC_POTENTIAL_RE = re.compile(": loop was not vectorized: inner loop was already vectorized")

# [2.4] remarks identifying dependencies present in loops 
LOOP_DEPENCENCE_RE = re.compile("loop was not parallelized: existence of parallel dependence")
LOOP_VECTOR_DEPENDENCE_RE = re.compile("loop was not vectorized: vector dependence prevents vectorization")

# [2.5] remarks identifying different sorts of applied 
# loop optimizations

# [2.5.0] (no loop optimizations reported)
LOOP_NO_OPTIMIZATIONS_RE = re.compile("No loop optimizations reported")

# [2.5.1] (loop-fusion)
FUSED_LOOPS_RE = re.compile("Fused loops: \((.*)\)")

# [2.5.2] (loop-distribution)
DISTRIBUTED_LOOP_RE = re.compile(": Loop Distributed \(([0-9]+) way\)")
DISTRIBUTED_CHUNK_N_RE = re.compile("<Distributed chunk([0-9]+)>")


# [2.6] Remarks, which do not affect loop parallelisability report (loop remainders, 
# peeled off loop parts, etc.) -> we just drop these types of loops out of report
LOOP_REMAINDER_VECTORIZATION_RE = re.compile("Remainder loop for vectorization")
LOOP_REMAINDER_1_RE = re.compile("Remainder")
LOOP_PEELED_RE = re.compile("Peeled loop for vectorization")
LOOP_FUSION_LOST_RE = re.compile("Loop lost in Fusion")

if __name__ != "__main__":
    
    re_loop_identification = {}
    re_loop_identification["begin"] = LOOP_BEGIN_RE
    re_loop_identification["end"] = LOOP_END_RE
    re_loop_identification["loop-name"] = LOOP_NAME_RE


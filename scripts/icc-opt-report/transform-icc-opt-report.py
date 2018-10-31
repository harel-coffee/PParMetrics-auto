#! /usr/bin/python3

import re
import sys
from enum import Enum

import icc_opt_report_regex
import icc_opt_report_regex

class Classification(Enum):
    NO = 0
    YES = 1
    UNSTATED = 2
    UNINITIALIZED = 3

class LoopInfo:
    """ All the information derived out of Intel C/C++ Compiler report """
    def __init__(self, name, filename, line, depth):
        # loop's location in the loop nesting structure and its name
        self.name = name
        self.filename = filename
        self.line = line
        self.depth = depth
        self.inner_loops = {}

        # 1) loop parallelisability overall status
        
        self.parallel = Classification.UNINITIALIZED
        
        # 2) loop parallelisability status

        # loop vectorization status
        self.vec = Classification.UNINITIALIZED
        # loop could potentially be vectorized, but wasn't
        # (inner loop was targeted for vectorization, for example)
        self.vec_potential = Classification.UNINITIALIZED
        # loop's parallelisation status
        self.par = Classification.UNINITIALIZED
        # loop could potentially be parallelized, but wasn't
        self.par_potential = Classification.UNINITIALIZED
        # #pragma omp presence
        self.openmp = Classification.UNINITIALIZED
        # loop dependence existence
        self.dep = Classification.UNINITIALIZED
        # loop has peeled off parts, split off parts, remainders, leftovers, etc.
        self.remainder = Classification.UNINITIALIZED
       
        # 3) loop optimization status
        
        # loop fusion
        self.fused = Classification.UNINITIALIZED
        self.fused_with = []
        self.fused_lost = Classification.UNINITIALIZED
        # loop fission (distribution)
        self.distributed = Classification.UNINITIALIZED
        self.distr_parts_n = 0
        # loop collapsing
        self.collapsed = Classification.UNINITIALIZED
        self.collapsed_with = []
        self.collapsed_eliminated = Classification.UNINITIALIZED

    def print_loop_report():
        loop_report_str = self.name + ":"

        if (self.par == Classification.NO
            print(loop_name + ":1")
        elif (classification == 3) or (classification == 2):

    print(loop_report_str)

class LoopClassificationTable:
    """ All the information derived out of Intel C/C++ Compiler report """
    def __init__(self, table_detail_level):
        self.loops = {}
        self.table_detail_level = table_detail_level

    def add_loop(loop_info):
        if loop_info.name not in self.loops:
            self.loops[loop_info.name] = loop_info
            return True
        else:
            return False

    def print_table_header():
        report_header = ""
        # Loop parallelisability
        report_header += "loop-name"
        report_header += ":parallel"
        report_header += ":vector"
        report_header += ":parallel-potential"
        report_header += ":vector-potential"
        report_header += ":remainder"
        report_header += ":dependence"
        report_header += ":openmp-pragma"
        # Applied loop optimizations
        # (loop-fusion)
        report_header += ":fused"
        report_header += ":fused-with"
        # (loop-distribution)
        report_header += ":distr"
        report_header += ":distr-n"
        report_header += ":distr-chunk"
        report_header += ":distr-chunk-n"
        # (loop-collapsing)
        report_header += ":collapsed"
        report_header += ":collapsed-with"

        print(report_header)

class IccOptReport:

    """ All the information derived out of Intel C/C++ Compiler report """

    def __init__(self, report_filename):
        self.report_filename = report_filename

        if not os.path.exists(self.report_filename):
            error_str = "script could not find Intel C/C++ Compiler optimization report file: " + str(icc_report_file_name)
            sys.exit(error_str)

        self.report = open(self.report_filename, "r")

    def transform_into_class_table(self, loop_class_table):
        
        # read ICC optimization report file line by line, recursively build 
        # corresponding loop nesting structure and populate it with optimization
        # information out of ICC compiler report
        
        loop_depth = 0
        while True:
            line = self.report.readline()
            if line == "":
                break
            loop_begin_match = LOOP_BEGIN_RE.search(line)
            if loop_begin_match != None:
                loop_filename = loop_begin_match.group(1)
                loop_line = loop_begin_match.group(2)
                loop_name = loop_filename + "(" + loop_line + ")"
                if loop_name not in loop_classification:
                    loop_classification[loop_name] = LoopInformationRecord(loop_name, loop_filename, int(loop_line))
                    extract_loop_information(loop_name, loop_classification[loop_name])
            else:
                openmp_construct_match = OPENMP_CONSTRUCT_RE.search(line)
                if openmp_construct_match != None:
                    loop_name = openmp_construct_match.group(1) + "(" + openmp_construct_match.group(2) + ")"
                    line = icc_report_file.readline()
                    openmp_parallel_match = OPENMP_PARALLEL_RE.search(line)
                    if openmp_parallel_match != None:
                        if loop_name not in loop_classification:
                            loop_classification[loop_name] = 1 # OpenMP parallelized
                        else:
                        if loop_classification[loop_name] == 3 or loop_classification[loop_name] == 0:
                            loop_classification[loop_name] = 1 # OpenMP parallelized



def extract_loop_information(loop_name, loop_info):
    global loop_depth
    global icc_report_file
    global loop_classification_table
    loop_depth += 1
    
    fused_loops = []

    is_distr_chunk = False
    loop_distr_chunk_name = ""

    while True:
        line = icc_report_file.readline()

        loop_distr_chunk = DISTRIBUTED_CHUNK_N_RE.search(line)
        if loop_distr_chunk != None:
            is_distr_chunk = True
            loop_distr_chunk_name = loop_name + "-" + loop_distr_chunk.group(1)
            if loop_distr_chunk_name not in loop_classification_table:
                distr_chunk_loop_info = LoopInformationRecord(loop_distr_chunk_name, subloop_filename, int(subloop_line))
                loop_classification_table[loop_distr_chunk_name] = distr_chunk_loop_info
            continue
        

        # detected inner loop -> process it recursively
        loop_begin_match = LOOP_BEGIN_RE.search(line)
        if loop_begin_match != None:
            subloop_filename = loop_begin_match.group(1)
            subloop_line = loop_begin_match.group(2)
            subloop_name = loop_filename + "(" + loop_line + ")"
            if subloop_name not in loop_classification:
                loop_classification[subloop_name] = LoopInformationRecord(subloop_name, subloop_filename, int(subloop_line))
            extract_loop_information(subloop_name, loop_classification[subloop_name])
            continue
        
        # auto-parallelized loops
        # loop was auto-parallelized
        loop_parallelized_match = LOOP_PARALLELIZED_RE.search(line)
        if loop_parallelized_match != None:
            loop_info.par = Classification.YES 
            continue
        # distributed loop was auto-parallelized
        loop_parallelized_match = LOOP_DISTR_PARALLELIZED_RE.search(line)
        if loop_parallelized_match != None:
            loop_info.par = Classification.YES 
            continue
        # fused loop was auto-parallelized
        loop_parallelized_match = LOOP_FUSED_PARALLELIZED_RE.search(line)
        if loop_parallelized_match != None:
            loop_info.par = Classification.YES 
            continue
        # partial loop was auto-parallelized
        loop_parallelized_match = LOOP_PARTIAL_PARALLELIZED_RE.search(line)
        if loop_parallelized_match != None:
            loop_info.par = Classification.YES 
            continue
        # loop could potentially be parallelized, but was excluded as inner 
        # loops are not the target for parallelisation
        loop_parallelized_match = LOOP_INNER_PAR_POTENTIAL_RE.search(line)
        if loop_parallelized_match != None:
            loop_info.par_potential = Classification.YES 
            continue

        # vectorized loops
        # loop was vectorized
        loop_vectorized_match = LOOP_VECTORIZED_RE.search(line)
        if loop_vectorized_match != None:
            loop_info.vec = Classification.YES
            continue
        # distributed loop was vectorized
        loop_vectorized_match = LOOP_DISTR_VECTORIZED_RE.search(line)
        if loop_vectorized_match != None:
            loop_info.vec = Classification.YES
            continue
        # fused loop was vectorized
        loop_vectorized_match = LOOP_FUSED_VECTORIZED_RE.search(line)
        if loop_vectorized_match != None:
            loop_info.vec = Classification.YES
            continue
        # partial loop was vectorized
        loop_vectorized_match = LOOP_PARTIAL_VECTORIZED_RE.search(line)
        if loop_vectorized_match != None:
            loop_info.vec = Classification.YES
            continue
        # vectorizer aims at inner loops and this one was not 
        # vectorized as an outer one 
        loop_vectorized_match = LOOP_INNER_VEC_POTENTIAL_RE.search(line)
        if loop_vectorized_match != None:
            loop_info.vec_potential = Classification.YES
            continue

        # fused loops
        fused_loops_match = FUSED_LOOPS_RE.search(line)
        if fused_loops_match != None:
            fused_loops_str = fused_loops_match.group(1)
            fused_loops = [int(s) for s in fused_loops_str.split() if s.isdigit()]
            loop_info.fused = Classification.YES
            loop_info.fused_with = fused_loops
            continue
        fused_loops_match = LOOP_FUSION_LOST_RE.search(line)
        if fused_loops_match != None:
            loop_info.fused_lost = Classification.YES
            continue

        # distributed loop
        distr_loop_match = DISTRIBUTED_LOOP_RE.search(line)
        if fused_loops_match != None:
            fused_loops_str = fused_loops_match.group(1)
            fused_loops = [int(s) for s in fused_loops_str.split() if s.isdigit()]
            loop_info.fused = Classification.YES
            loop_info.fused_with = fused_loops
            continue
        fused_loops_match = LOOP_FUSION_LOST_RE.search(line)
        if fused_loops_match != None:
            loop_info.fused_lost = Classification.YES
            continue




        # loop remainder, split, peel etc. - do not carry classifying information
        loop_remainder_vec_match = LOOP_REMAINDER_VECTORIZATION_RE.search(line)
        loop_peeled_match = LOOP_PEELED_RE.search(line)
        if loop_remainder_vec_match != None or loop_peeled_match != None:
            loop_info.peeled = Classification.YES
            continue

        # loop is not parallelizible
        loop_non_parallel_match = LOOP_NON_PARALLEL_0_RE.search(line)
        if loop_non_parallel_match != None:
            if loop_classification[loop_name] == 3:
                loop_classification[loop_name] = 0 # non-parallelizible
            elif loop_classification[loop_name] == 1:
                loop_classification[loop_name] == 2
                print("Loop parallelisation ambiguity: " + str(loop_name))
            continue
        loop_non_parallel_match = LOOP_NON_VECTORIZABLE_0_RE.search(line)
        if loop_non_parallel_match != None:
            if loop_classification[loop_name] == 3:
                loop_classification[loop_name] = 0 # non-parallelizible
            elif loop_classification[loop_name] == 1:
                loop_classification[loop_name] == 2
                print("Loop parallelisation ambiguity: " + str(loop_name))
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

loop_classification_table_headers = {}

def print_loop_classification_table_header():
    
    
    
    
    
    report_header = ""
    # Loop parallelisability
    report_header += "loop-name"
    report_header += ":parallel"
    report_header += ":vector"
    report_header += ":parallel-potential"
    report_header += ":vector-potential"
    report_header += ":remainder"
    report_header += ":dependence"
    report_header += ":openmp-pragma"
    # Applied loop optimizations
    # (loop-fusion)
    report_header += ":fused"
    report_header += ":fused-with"
    # (loop-distribution)
    report_header += ":distr"
    report_header += ":distr-n"
    report_header += ":distr-chunk"
    report_header += ":distr-chunk-n"
    # (loop-collapsing)
    report_header += ":collapsed"
    report_header += ":collapsed-with"

    print(report_header)


def print_loop_report(loop_name, loop_inf_record):
    loop_report_str = loop_name + ":"

    if (loop_inf_record.par == True) and (loop_inf_record.vec):
        print(loop_name + ":1")
    elif (classification == 3) or (classification == 2):

    print(loop_report_str)

def print_icc_loop_table_report(loop_classifications):
    print_icc_loop_report_header()
    
    for name, classification in loop_classification.items():


if __name__ == "__main__":

    print("<= Intel C/C++ Compiler optimization report processing script =>")

    if len(sys.argv) != 3:
        error_str = "script usage error: use ./transform-icc-opt-report.py (icc_report_file) (approximation_num)\n"
        error_str += "where (icc_report_file) is the optimization report file produced by ICC compiler,\n"
        error_str += "and (approximation_num) is the level of detail to capture in the ICC report\n"
        sys.exit("error: " + error_str)

    icc_opt_report_filename = sys.argv[1]
    icc_opt_report = IccOptReport(icc_opt_report_filename)

    
    report_detail_level = int(sys.argv[2]) # the greater the level, the more precise report is
    if report_detail_level not in [1,2,3]:
        error_str = "the loop classification table detail level (detail-level) must be in [1,3] range!"
        sys.exit("error: " + error_str)

    icc_report_file = open(sys.argv[1],"r")
    
    loop_class_table = LoopClassificationTable(report_detail_level)
    loop_depth = 0
   
    # read ICC optimization report file line by line, recursively build 
    # corresponding loop nesting structure and populate it with optimization
    # information out of ICC compiler report
    while True:
        line = icc_report_file.readline()
        if line == "":
            break
        loop_begin_match = LOOP_BEGIN_RE.search(line)
        if loop_begin_match != None:
            loop_filename = loop_begin_match.group(1)
            loop_line = loop_begin_match.group(2)
            loop_name = loop_filename + "(" + loop_line + ")"
            if loop_name not in loop_classification:
                loop_classification[loop_name] = LoopInformationRecord(loop_name, loop_filename, int(loop_line))
                extract_loop_information(loop_name, loop_classification[loop_name])
        else:
            openmp_construct_match = OPENMP_CONSTRUCT_RE.search(line)
            if openmp_construct_match != None:
                loop_name = openmp_construct_match.group(1) + "(" + openmp_construct_match.group(2) + ")"
                line = icc_report_file.readline()
                openmp_parallel_match = OPENMP_PARALLEL_RE.search(line)
                if openmp_parallel_match != None:
                    if loop_name not in loop_classification:
                        loop_classification[loop_name] = 1 # OpenMP parallelized
                    else:
                        if loop_classification[loop_name] == 3 or loop_classification[loop_name] == 0:
                            loop_classification[loop_name] = 1 # OpenMP parallelized

    print_icc_loop_report(loop_classification)
    for name, classification in loop_classification.items():
        

    print("Done!")

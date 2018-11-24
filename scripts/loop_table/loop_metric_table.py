#! /usr/bin/python3

import re
import sys

class LoopMetricTable:

    """ 
    Benchmark Loop Metrics Table contains all the calculated loop metrics 
    """
    
    def __init__(self):
        self.loops = {} # loops{"filename.c(#line)"} => LoopClassificationInfo

    def add_loop_entry(loop_name):
        if loop_name not in self.loops:
            self.loops[loop_name] = None
            return True
        else:
            return False

    def add_loop_classification(loop_class_info):
        if loop_class_info.name not in self.loops:
            self.loops[loop_class_info.name] = loop_class_info
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

    def print_table_content():

        for loop_name, loop_class in self.loops.items():
            # loop name
            print(loop_name + ":")
            # loop class per column 
            if loop_class.is_parallel(): 
                print("1:")
            else:
                print("0:")

            if loop_class.is_parallel_potential(): 
                print("1:")
            else:
                print("0:")
            
            if loop_class.is_vector(): 
                print("1:")
            else:
                print("0:")
    
    def print():
        print_table_header()
        print_table_content()
    
if __name__ == "__main__":
    print("Done!")
else:
    pass

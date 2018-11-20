#! /usr/bin/python3

import sys
import os

from icc.opt_report.compiler import IccOptReportCompiler
from icc.opt_report.ir import *
from loop_table.loop_class_table import LoopClassTable

if __name__ == "__main__":

    print("= Intel C/C++ Compiler (ICC) optimization report processing =")
    print("ICC opt report -> Loop classification table\n")

    if len(sys.argv) != 2:
        error_str = "error: "
        error_str += "ICC opt report procesor: "
        error_str += "incorrect argument list => use ./process-icc-report.py opt-report-filename"
        sys.exit(error_str)

    compiler = IccOptReportCompiler(sys.argv[1])
    compiler.compile()
    loop_ir = compiler.get_ir()
    loops = loop_ir.get_loops()
   
    loop_class_table = LoopClassTable()
    
    for loop_name in loops:
        loop_class_table.add_loop(loops[loop_name])

    loop_class_table.print_black_box()

    sys.exit()

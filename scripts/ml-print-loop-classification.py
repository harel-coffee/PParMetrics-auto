#! /usr/bin/python3

import sys

from intel_compiler.opt_report import IccOptReport
from loop_table import LoopTable

if __name__ == "__main__":

    print("Intel C/C++ Compiler optimization report processing script")

    if len(sys.argv) != 3:
        sys.exit("error: 3 args")

    opt_report_filename = sys.argv[1]
    opt_report = IccOptReport(opt_report_filename)
    
    loop_tb = opt_report.create_loop_table()
    
    loop_table_filename = sys.argv[2]
    loop_tb.print_into_file(loop_table_filename)

    sys.exit(0)

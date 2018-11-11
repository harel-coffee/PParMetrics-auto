#! /usr/bin/python3

import re
import sys

from intel_compiler.opt_report.compiler import IccOptReportCompiler
from intel_compiler.opt_report.ir import *
from benchmark_loop_tables.loop_class_table import LoopClassTable

if __name__ == "__main__":

    icc_report_filename = sys.argv[1]

    loop_class_table = LoopClassTable()

    compiler = IccOptReportCompiler(icc_report_filename)

    compiler.compile()

    loop_nest_structure = compiler.get_ir()

    print("Done!")

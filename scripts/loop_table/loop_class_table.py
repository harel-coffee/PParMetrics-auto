#! /usr/bin/python3

import re
import sys

from icc.opt_report.ir import *

class ConservativeClassifier:

    """ 
    Conservative Classifier is afraid of overrating loop's parallelisability 
    """

    def __init__(self):
        pass
    
    def classify_loop(self, loop):
        loop_class = loop.classification
            
        if ( loop_class.parallel_dependence.value == Classification.YES.value or 
             loop_class.parallel_not_candidate.value == Classification.YES.value or 
             loop_class.vector_dependence.value == Classification.YES.value ):
            return Classification.NO
            
        if ( loop_class.parallel.value == Classification.YES.value or 
             loop_class.memset.value == Classification.YES.value or
             loop_class.vector.value == Classification.YES.value ):
            return Classification.YES

        return Classification.NO

    def classify(self, loop):
        
        ret = Classification.YES

        if len(loop.distr_chunks) != 0:
            if loop.classification.distr.value != Classification.YES.value:
                pass
                #sys.exit("error: loop class table: detected IR inconsistency!")
            
            for distr_chunk_num in loop.distr_chunks:
                distr_chunk = loop.distr_chunks[distr_chunk_num]
                if self.classify_loop(distr_chunk).value != Classification.YES.value:
                    ret = Classification.NO
            
            return ret
        else:
            return self.classify_loop(loop)

class BiasedClassifier:

    """ 
    Biased Classifier wants to see as many parallel loops, as possible
    """

    def __init__(self):
        pass

    def classify_loop(self, loop):
        loop_class = loop.classification

        if ( loop_class.parallel_dependence.value == Classification.YES.value or 
             loop_class.parallel_not_candidate.value == Classification.YES.value or 
             loop_class.vector_dependence.value == Classification.YES.value ):
            return Classification.NO

        if ( loop_class.parallel.value == Classification.YES.value or 
             loop_class.memset.value == Classification.YES.value or
             loop_class.vector.value == Classification.YES.value ):
            return Classification.YES

        if ( loop_class.parallel_potential.value == Classification.YES.value or 
             loop_class.vector_potential.value == Classification.YES.value ):
            return Classification.YES

        return Classification.NO

    def classify(self, loop):
        
        ret = Classification.NO

        if len(loop.distr_chunks) != 0:
            if loop.classification.distr.value != Classification.YES.value:
                pass
                #sys.exit("error: loop class table: detected IR inconsistency for loop " + loop.name + "!")
            
            for distr_chunk_num in loop.distr_chunks:
                distr_chunk = loop.distr_chunks[distr_chunk_num]
                if self.classify_loop(distr_chunk).value == Classification.YES.value:
                    ret = Classification.YES
                    return ret
            
            return ret
        else:
            return self.classify_loop(loop)

class LoopClassTable:

    """ 
    Benchmark Loop Classification Table contains all the information derived out of Intel C/C++ Compiler report 
    """
    
    def __init__(self):
        self.loops = {} # loops{"filename.c(#line)"} => LoopClassificationInfo
        self.biased_classifier = BiasedClassifier()
        self.conservative_classifier = ConservativeClassifier()

    def add_loop(self, loop):
        if loop.name not in self.loops:
            self.loops[loop.name] = loop
            return True
        else:
            return False

    def print_black_box(self):
        self.print_table_header_bb()
        self.print_table_content_bb()
        return

    def print_table_header_bb(self):

        # header 1
        report_header = ""
        report_header += "loop"
        report_header += ":classification"
        report_header += ":remarks"
        report_header += ":optimizations"
        print(report_header)

        # header 2
        report_header = ""
        # loop identification
        report_header += "src-loop"
        report_header += ":distr"
        # loop classifications
        report_header += ":biased"
        report_header += ":conservative"
        # loop remarks
        report_header += ":par"
        report_header += ":par-potential"
        report_header += ":vec"
        report_header += ":vec-potential"
        report_header += ":memset"
        report_header += ":dep-vec"
        report_header += ":dep-par"
        report_header += ":par-no-cand"
        # loop optimizations
        report_header += ":tiled"
        report_header += ":fused"
        report_header += ":collapsed"
        print(report_header)

    def print_loop_row_bb(self, loop, distributed):
        loop_class = loop.classification
        loop_row = ""

        # loop identification
        if distributed == False:
            loop_row += loop.name + ":"
            if loop_class.distr.value == Classification.YES.value: 
                loop_row += "1:"
            else: 
                loop_row += "0:"
        else:
            loop_row += ":" + "distr-chunk-" + str(loop.number) + ":"

        # loop classification            
        if self.biased_classifier.classify(loop).value == Classification.YES.value:
            loop_row += "1:"
        else: 
            loop_row += "0:"
           
        if self.conservative_classifier.classify(loop).value == Classification.YES.value:
            loop_row += "1:"
        else: 
            loop_row += "0:"

        # loop remarks            
        if loop_class.parallel.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"
            
        if loop_class.parallel_potential.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"

        if loop_class.vector.value == Classification.YES.value:
            loop_row += "1:"
        else: 
            loop_row += "0:"
            
        if loop_class.vector_potential.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"
 
        if loop_class.memset.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"
           
        if loop_class.vector_dependence.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"
 
        if loop_class.parallel_dependence.value == Classification.YES.value:
            loop_row += "1:"
        else: 
            loop_row += "0:"
 
        if loop_class.parallel_not_candidate.value == Classification.YES.value:
            loop_row += "1:"
        else: 
            loop_row += "0:"
          
        # loop optimizations
        if loop_class.tiled.value == Classification.YES.value: 
            loop_row += "1:"
        else: 
            loop_row += "0:"

        if ( loop_class.fused.value == Classification.YES.value or
             loop_class.fused_lost.value == Classification.YES.value ): 
            loop_row += "1:"
        else: 
            loop_row += "0:"

        if ( loop_class.collapsed.value == Classification.YES.value or
             loop_class.collapse_eliminated.value == Classification.YES.value ):
            loop_row += "1:"
        else: 
            loop_row += "0:"

        print(loop_row)

    def print_table_content_bb(self):

        for loop_name in self.loops:
            loop = self.loops[loop_name]
            self.print_loop_row_bb(loop, False)

            if len(loop.distr_chunks) != 0:
                if loop.classification.distr.value != Classification.YES.value:
                    pass
                    #sys.exit("error: loop class table: detected IR inconsistency for loop " + loop.name + "!")
                for distr_chunk_num in loop.distr_chunks:
                    distr_chunk = loop.distr_chunks[distr_chunk_num]
                    self.print_loop_row_bb(distr_chunk, True)
       
if __name__ == "__main__":
    print("Done!")
else:
    pass

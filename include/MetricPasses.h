#ifndef PPAR_METRIC_PASSES_H
#define PPAR_METRIC_PASSES_H

#include "ppar_common_includes.h"

#include <string>

namespace ppar {

struct LoopPayloadFraction {
    public:
        LoopPayloadFraction() {}

        static llvm::StringRef getPassName()
        { return "Loop Payload Fraction Metric Pass"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-payload-fraction"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "Compute the fraction of payload (number of instructions) to the total size of the loop"; }
};

struct LoopProperSCCsNumber {
    public:
        LoopProperSCCsNumber() {}

        static llvm::StringRef getPassName()
        { return "Loop Proper SCCs Number Metric Pass"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-proper-sccs-num"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The number of SCCs of size greater than 2 instructions in the loop"; }
};

/*
 * Loop parts cohesion metrics.
 * 
 * In this work loop is composed of several parts:
 * 
 * Loop    := Iterator Payload
 * Payload := CriticalSCCs RegularSCCs
 *
 */

struct IteratorPayloadTotalCohesion {
    public:
        IteratorPayloadTotalCohesion() {}

        static llvm::StringRef getPassName()
        { return "Iterator-Payload Cohesion"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-iter-payload-cohesion"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The ratio between the number of dependence edges between\
                  iterator and payload components of the loop and the total number of edges in the loop"; }
};

struct IteratorPayloadNonCFCohesion {
    public:
        IteratorPayloadNonCFCohesion() {}

        static llvm::StringRef getPassName()
        { return "Iterator-Payload Cohesion"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-iter-payload-cohesion"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The ratio between the number of dependence edges between\
                  iterator and payload components of the loop and the total number of edges in the loop"; }
};

struct IteratorPayloadMemoryCohesion {
    public:
        IteratorPayloadMemoryCohesion() {}

        static llvm::StringRef getPassName()
        { return "Iterator-Payload Cohesion"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-iter-payload-cohesion"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The ratio between the number of dependence edges between\
                  iterator and payload components of the loop and the total number of edges in the loop"; }
};

struct LoopAbsoluteSize {
    public:
        LoopAbsoluteSize() {}

        static llvm::StringRef getPassName()
        { return "Loop Absolute Size"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-absolute-size"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The absolute size of the loop in tens of instructions (instructions number / 10)"; }
};

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASSES_H

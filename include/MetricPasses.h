#ifndef PPAR_METRIC_PASSES_H
#define PPAR_METRIC_PASSES_H

#include "ppar_common_includes.h"

#include <string>

namespace ppar {

struct IteratorLoopPercentage {
    public:
        IteratorLoopPercentage() {}

        static llvm::StringRef getPassName()
        { return "Iterator Loop Percentage Metric Pass"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "ppar-iterator-loop-percentage"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "Compute the size of the loop iterator relative to the whole loop size for all loops of the given function F"; }
};

struct LoopSCCsNumber {
    public:
        LoopSCCsNumber() {}

        static llvm::StringRef getPassName()
        { return "Loop SCCs Number Metric Pass"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-sccs-num"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The number of SCCs of size greater than 2 instructions in the loop"; }
};

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASSES_H

#ifndef PPAR_METRIC_PASSES_H
#define PPAR_METRIC_PASSES_H

#include "ppar_common_includes.h"
#include <string>

namespace ppar {

struct IteratorPayloadRatioMetricPass {
    public:
        IteratorPayloadRatioMetricPass() {}

        static llvm::StringRef getPassName()
        { return "Iterator/Payload Ratio Metric Pass"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "iterator-payload-metric"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "Compute Iterator/Payload Ratio metric for all loops of the given function F"; }
};

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASSES_H

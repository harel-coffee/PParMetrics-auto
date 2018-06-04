#ifndef PPAR_METRIC_PASSES_H
#define PPAR_METRIC_PASSES_H

#include "ppar_common_includes.h"

#include <string>

namespace ppar {

struct LoopProportionMetrics {
    public:
        LoopProportionMetrics() {}

        typedef enum ProportionMetricType {
            LOOP_ABSOLUTE_SIZE = 0, // the number of instructions in the loop
            LOOP_PAYLOAD_FRACTION, // the fraction payload constitutes of the whole loop
            LOOP_PROPER_SCCS_NUMBER, // the number of SCCs in the payload of the loop, with the amount of instructions greater than 1
            METRIC_SUBTYPE_LAST
        } ProportionMetric_t;

        using MetricSubtype = ProportionMetric_t;

        static llvm::StringRef getPassName()
        { return "Loop Proportions Metrics"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-proportions-metrics"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "Compute different proportions of the loop\
                  (absolute size in the number of instructions, iterator to payload ration, etc."; }
};

struct LoopCohesionMetrics {
    public:
        LoopCohesionMetrics() {}
        
        typedef enum CohesionMetricType {
            // iterator <-> payload
            ITERATOR_PAYLOAD_TOTAL_COHESION = 0, // the total amount of edges between loop iterator and payload
            ITERATOR_PAYLOAD_NON_CF_COHESION, // the amount of non-control-flow edges between loop iterator and payload
            ITERATOR_PAYLOAD_MEM_COHESION, // the amount of memory edges between loop iterator and payload
            // payload critical sccs <-> payload regular sccs
            PAYLOAD_CRITICAL_TOTAL_COHESION,
            PAYLOAD_CRITICAL_NON_CF_COHESION,
            PAYLOAD_CRITICAL_MEM_COHESION,
            METRIC_SUBTYPE_LAST
        } CohesionMetric_t;

        using MetricSubtype = CohesionMetric_t;

        static llvm::StringRef getPassName()
        { return "Loop Cohesion Metrics"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-cohesion-metrics"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The set of metrics measuring cohesion between different parts of the loop (iterator,\
                  payload, critical SCCs, etc)"; }
};

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASSES_H

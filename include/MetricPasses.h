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
            LOOP_ITERATOR_SIZE, // the number of instructions in the loop's iterator
            LOOP_PAYLOAD_FRACTION, // the fraction payload constitutes of the whole loop
            LOOP_PROPER_SCCS_NUMBER, // the number of SCCs in the payload of the loop, with the amount of instructions greater than 1
            LOOP_CRITICAL_PAYLOAD_FRACTION, // the fraction critical part of the payload constitutes of the whole payload
            // in case if this loop relates to the loop nest
            LOOP_DEPTH, // the depth of the loop in a loop nest
            LOOP_INNER_LOOP_NUM, // the number of inner loops inside the given loop
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
            CRITICAL_PAYLOAD_TOTAL_COHESION,
            CRITICAL_PAYLOAD_NON_CF_COHESION,
            CRITICAL_PAYLOAD_MEM_COHESION,
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

struct LoopDependenceMetrics {
    public:
        LoopDependenceMetrics() {}
        
        typedef enum DependenceMetricType {
            ITERATOR_TOTAL_DEPENDENCIES_NUM = 0,
            ITERATOR_TRUE_DEPENDENCIES_NUM,
            ITERATOR_ANTI_DEPENDENCIES_NUM,
            ITERATOR_OUTPUT_DEPENDENCIES_NUM,
            ITERATOR_CROSS_DEPENDENCIES_NUM,
            ITERATOR_CONF_DEPENDENCIES_NUM,
            ITERATOR_REG_DEPENDENCIES_NUM,
            ITERATOR_MEM_DEPENDENCIES_NUM,
            ITERATOR_CNTL_DEPENDENCIES_NUM,
            PAYLOAD_TOTAL_DEPENDENCIES_NUM,
            PAYLOAD_TRUE_DEPENDENCIES_NUM,
            PAYLOAD_ANTI_DEPENDENCIES_NUM,
            PAYLOAD_OUTPUT_DEPENDENCIES_NUM,
            PAYLOAD_CROSS_DEPENDENCIES_NUM,
            PAYLOAD_CONF_DEPENDENCIES_NUM,
            PAYLOAD_REG_DEPENDENCIES_NUM,
            PAYLOAD_MEM_DEPENDENCIES_NUM,
            PAYLOAD_CNTL_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_TOTAL_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_TRUE_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_ANTI_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_OUTPUT_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_CROSS_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_CONF_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_REG_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_MEM_DEPENDENCIES_NUM,
            CRITICAL_PAYLOAD_CNTL_DEPENDENCIES_NUM,
            METRIC_SUBTYPE_LAST
        } DependenceMetric_t;

        using MetricSubtype = DependenceMetric_t;

        static llvm::StringRef getPassName()
        { return "Loop Dependence Metrics"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-dependence-metrics"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "The set of metrics measuring the amount of different types of dependencies in the payload of the loop"; }
};

struct LoopNatureMetrics {
    public:
        LoopNatureMetrics() {}

        typedef enum ProportionMetricType {
            ITERATOR_MEM_WRITE_COUNT = 0, // the number of may-write-to-memory instructions in the loop iterator
            ITERATOR_MEM_WRITE_FRACTION, // the fraction of may-write-to-memory instructions in the loop iterator out of all iterator instructions
            ITERATOR_MEM_READ_COUNT, // the number of may-read-from-memory instructions in the loop iterator
            ITERATOR_MEM_READ_FRACTION, // the fraction of may-read-from-memory instructions in the loop iterator out of all iterator instructions
            ITERATOR_CALL_COUNT, // the number of call instructions in the loop iterator
            ITERATOR_CALL_FRACTION, // the fraction of call instructions in the loop iterator
            ITERATOR_BRANCH_COUNT, // the number of branch instructions in the loop iterator
            ITERATOR_BRANCH_FRACTION, // the fraction of branch instructions in the loop iterator
            ITERATOR_GETELEMPTR_COUNT, // the number of getelementptr instructions in the loop iterator
            ITERATOR_GETELEMPTR_FRACTION, // the fraction of getelementptr instructions in the loop iterator
            PAYLOAD_MEM_WRITE_COUNT,
            PAYLOAD_MEM_WRITE_FRACTION,
            PAYLOAD_MEM_READ_COUNT,
            PAYLOAD_MEM_READ_FRACTION,
            PAYLOAD_CALL_COUNT,
            PAYLOAD_CALL_FRACTION,
            PAYLOAD_BRANCH_COUNT,
            PAYLOAD_BRANCH_FRACTION,
            PAYLOAD_GETELEMPTR_COUNT,
            PAYLOAD_GETELEMPTR_FRACTION,
            CRITICAL_PAYLOAD_MEM_WRITE_COUNT,
            CRITICAL_PAYLOAD_MEM_WRITE_FRACTION,
            CRITICAL_PAYLOAD_MEM_READ_COUNT,
            CRITICAL_PAYLOAD_MEM_READ_FRACTION,
            CRITICAL_PAYLOAD_CALL_COUNT,
            CRITICAL_PAYLOAD_CALL_FRACTION,
            CRITICAL_PAYLOAD_BRANCH_COUNT,
            CRITICAL_PAYLOAD_BRANCH_FRACTION,
            CRITICAL_PAYLOAD_GETELEMPTR_COUNT,
            CRITICAL_PAYLOAD_GETELEMPTR_FRACTION,
            METRIC_SUBTYPE_LAST
        } NatureMetric_t;

        using MetricSubtype = NatureMetric_t;

        static llvm::StringRef getPassName()
        { return "Loop Instruction Nature Metrics"; } 
        
        static llvm::StringRef getMetricPassOption()
        { return "loop-nature-metrics"; }
        
        static llvm::StringRef getMetricPassOptionDesc()
        { return "Compute metrics, which take into account the nature of\
                  loop constituting instructions (call instructions number, memory writes number, etc.)"; }
};

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASSES_H

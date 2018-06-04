#ifndef PPAR_METRIC_PASS_IMPL_H
#define PPAR_METRIC_PASS_IMPL_H

#include "MetricPass.h"

#define RegisterMetricPass(METRIC,VAR) \
\
RegisterPass<ppar::MetricPass<METRIC>> \
VAR( METRIC::getMetricPassOption(),\
     METRIC::getMetricPassOptionDesc() );

namespace ppar {

template <typename METRIC>
char MetricPass<METRIC>::ID = 0;

template <typename METRIC>
MetricPass<METRIC>::MetricPass() : FunctionPass(ID) {
    FunctionMetrics.clear();
}

template <typename METRIC>
MetricPass<METRIC>::~MetricPass() {
    FunctionMetrics.clear();
}

template <typename METRIC>
void MetricPass<METRIC>::releaseMemory() {}

template <typename METRIC>
void MetricPass<METRIC>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    llvm_unreachable("Class template MetricPass cannot be used directly (without concrete specialization)!");
    AU.setPreservesAll();
}

template <typename METRIC>
llvm::StringRef MetricPass<METRIC>::getPassName() const { 
    return METRIC::getPassName(); 
}

template <typename METRIC>
bool MetricPass<METRIC>::runOnFunction(llvm::Function& F) {
    llvm_unreachable("Class template MetricPass cannot be used directly (without concrete specialization)!");
}

} // namespace ppar

#endif // #ifndef PPAR_METRIC_PASS_IMPL_H

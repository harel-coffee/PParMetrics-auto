#ifndef PPAR_METRICS_ANALYSIS_H
#define PPAR_METRICS_ANALYSIS_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "ppar-metrics"

namespace {
   
    class PParMetrics : public FunctionPass {
    public:
        static char ID; // Pass identification, replacement for typeid

        PParMetrics();
        bool runOnFunction(Function& F) override;

        void getAnalysisUsage(AnalysisUsage &AU) const override;
    };

} // anonymous namespace

#endif // #ifndef PPAR_METRICS_ANALYSIS_H

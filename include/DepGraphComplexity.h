#ifndef PPAR_DEP_GRAPH_COMPLEXITY_H
#define PPAR_DEP_GRAPH_COMPLEXITY_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/Statistic.h"
using namespace llvm;

#include "DependenceGraph.h"
#include "ProgramDependenceGraph.h"

namespace ppar {
   
    class DepGraphComplexity : public FunctionPass {
    public:
        static char ID; 
        DepGraphComplexity();
        bool runOnFunction(Function& F) override;
        void getAnalysisUsage(AnalysisUsage& AU) const override;
    };

} // namespace ppar

#endif // #ifndef PPAR_DEP_GRAPH_COMPLEXITY_H

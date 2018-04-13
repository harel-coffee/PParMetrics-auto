#ifndef SCCS_GRAPH_PASS_H
#define SCCS_GRAPH_PASS_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/Statistic.h"
using namespace llvm;

namespace ppar {
   
    class SCCsGraphPass : public FunctionPass {
    public:
        static char ID; 
        SCCsGraphPass();
        bool runOnFunction(Function& F) override;
        void getAnalysisUsage(AnalysisUsage& AU) const override;
        
        const Graph<llvm::Instruction*,ppar::Dependence*>& getPDG() const;
    };

} // namespace ppar

#endif // #ifndef SCCS_GRAPH_PASS_H

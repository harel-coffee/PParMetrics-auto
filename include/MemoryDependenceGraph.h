#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Analysis/DependenceAnalysis.h"
using namespace llvm;

#include "DependenceGraph.h"

namespace ppar {

class MemoryDependenceGraphPass : public FunctionPass {
    public:
        static char ID;

        MemoryDependenceGraphPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override;

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.setPreservesAll();
            AU.addRequired<DependenceAnalysisWrapperPass>();
        }
    
        StringRef getPassName() const override { return "Memory Dependence Graph Pass"; }

        void releaseMemory() override { MDG.clear(); }

        DependenceGraph<Instruction*,Dependence*>& getMDG() { return MDG; }
    
    private:
        DependenceGraph<Instruction*,Dependence*> MDG;
};

} // namespace ppar

#endif // #ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_H

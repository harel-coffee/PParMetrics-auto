#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_H

#include "DependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/DependenceAnalysis.h"

#include <memory>

namespace ppar {

struct MemoryDependenceGraphPass : public llvm::FunctionPass {
    public:
        static char ID;
        MemoryDependenceGraphPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const override;
        void releaseMemory() override;
        DependenceGraph<llvm::Instruction*,llvm::Dependence*>& getMDG();
    
    private:
        DependenceGraph<llvm::Instruction*,llvm::Dependence*> MDG;
};

} // namespace ppar

#endif // #ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_H

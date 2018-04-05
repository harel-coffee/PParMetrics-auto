#ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_H
#define PPAR_CONTROL_DEPENDENCE_GRAPH_H

#include "ppar_common_includes.h"
#include "DependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DependenceAnalysis.h"

#include <memory>

namespace ppar {

struct ControlDependenceGraphPass : public llvm::FunctionPass {
    public:
        static char ID;
        ControlDependenceGraphPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const override;
        void releaseMemory() override;
        const DependenceGraph<llvm::BasicBlock*,ppar::Dependence*>& getCDG() const;

    private:
        DependenceGraph<llvm::BasicBlock*,ppar::Dependence*> CDG;
};

} // namespace ppar

#endif // #ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_H

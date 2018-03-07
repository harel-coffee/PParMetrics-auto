#ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_H
#define PPAR_PROGRAM_DEPENDENCE_GRAPH_H

#include "DependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"

namespace ppar {

struct ProgramDependenceGraphPass : public llvm::FunctionPass {
    public:
        static char ID;
        ProgramDependenceGraphPass() : FunctionPass(ID) {}

        bool runOnFunction(llvm::Function &F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const override;
        void releaseMemory() override;
        const DependenceGraph<llvm::Instruction*,ppar::Dependence*>& getPDG() const;

    private:
        DependenceGraph<llvm::Instruction*,ppar::Dependence*> PDG;
};

} // namespace ppar

#endif // #ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_H

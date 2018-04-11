#ifndef PPAR_DATA_DEPENDENCE_GRAPH_H
#define PPAR_DATA_DEPENDENCE_GRAPH_H

#include "ppar_common_includes.h"
#include "Graph.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"

#include <vector>

namespace ppar {

struct DataDependenceGraphPass : public llvm::FunctionPass {
    public:
        static char ID;
        DataDependenceGraphPass();

        bool runOnFunction(llvm::Function &F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const override;
        void releaseMemory() override;
        Graph<llvm::Instruction*,ppar::Dependence*>& getDDG();

    private:
        Graph<llvm::Instruction*,ppar::Dependence*> DDG;
};

} // namespace ppar

#endif // #ifndef PPAR_DATA_DEPENDENCE_GRAPH_H

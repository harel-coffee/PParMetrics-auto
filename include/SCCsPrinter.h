#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H

#include "ProgramDependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct PDGPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        PDGPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

} // namespace ppar

#endif // ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H

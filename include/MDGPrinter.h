#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H

#include "ppar_common_includes.h"
#include "MemoryDependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct MDGPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        MDGPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

} // namespace ppar

#endif // ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_H

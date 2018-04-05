#ifndef PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_H
#define PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_H

#include "ppar_common_includes.h"
#include "DataDependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct DDGPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        DDGPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

} // namespace ppar

#endif // ifndef PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_H

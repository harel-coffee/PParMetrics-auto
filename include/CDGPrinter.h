#ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PRINTER_H
#define PPAR_CONTROL_DEPENDENCE_GRAPH_PRINTER_H

#include "ControlDependenceGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct CDGPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        CDGPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

} // namespace ppar

#endif // ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PRINTER_H

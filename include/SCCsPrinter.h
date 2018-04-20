#ifndef PPAR_SCCS_GRAPH_PRINTER_H
#define PPAR_SCCS_GRAPH_PRINTER_H

#include "SCCsGraph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct SCCsPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        SCCsPrinter();

        void printSCCs(Function& F);
        void printComponentGraph(Function& F);

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

} // namespace ppar

#endif // #ifndef PPAR_SCCS_GRAPH_PRINTER_H

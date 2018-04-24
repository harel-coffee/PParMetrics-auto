#ifndef PPAR_GRAPH_PRINTER_H
#define PPAR_GRAPH_PRINTER_H

#include "ppar_common_includes.h"
#include "DotPrinter.h"
#include "Graph.h"

#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

#include <string>

namespace ppar {

template <typename NODE, typename EDGE, typename PASS>
struct GraphPrinter : public llvm::FunctionPass {

    public:
        static char ID;
        GraphPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override {}

        typedef enum class GraphPrintType {
            PRINT_REGULAR,
            PRINT_DFS
        } PrintType;

    private:
        void printDOTGraph(llvm::Function& F, PrintType Type = PrintType::PRINT_REGULAR);
        void printSCCsDOTGraph(llvm::Function& F);
        void printComponentGraph(llvm::Function& F);

        void buildDotNode(NODE N, DotNode* Node);
        void buildDotEdge(EDGE Dep, DotEdge* Edge); 
};

} // namespace ppar

#endif // ifndef PPAR_GRAPH_PRINTER_H

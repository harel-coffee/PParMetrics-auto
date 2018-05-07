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
struct GraphPrinter;

template <typename NODE, typename EDGE, typename PASS>
struct GraphPrinter<NODE*,EDGE*,PASS> : public llvm::FunctionPass {

    public:
        static char ID;
        GraphPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override {}

        typedef enum class PrintAugmentType {
            PRINT_AUGMENT_NO,
            PRINT_AUGMENT_DFS_TIMESTAMPS
        } PrintAugmentType;

        typedef enum class GraphPrintType {
            PRINT_MAIN_GRAPH,
            PRINT_SCC_SUBGRAPH,
            PRINT_COMPONENT_GRAPH
        } PrintType;

    private:
        void formDOTGraph(DotGraph&, const Graph<NODE*,EDGE*>&, 
                          PrintType Type = PrintType::PRINT_MAIN_GRAPH, 
                          PrintAugmentType AugmentType = PrintAugmentType::PRINT_AUGMENT_NO);

        void buildDotNode(const NODE* N, DotNode* Node);
        void buildDotEdge(const EDGE* Dep, DotEdge* Edge); 
};

} // namespace ppar

#endif // ifndef PPAR_GRAPH_PRINTER_H

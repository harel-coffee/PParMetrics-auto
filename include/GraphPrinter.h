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

        typedef enum GraphFormationOption {
            DEFAULT = 0x0,
            DFS_TIMESTAMPS = 0x1, // add DFS timestamps to the nodes of the graph
            ONLY_NODES = 0x2, // do not include graph edges into the DOT file
            ONLY_EDGES = 0x4, // do not include graph nodes into the DOT file
            MARK_SCC_ROOTS = 0x8
        } FormationOption;

    private:
        void printDotGraphs(const Graph<NODE*,EDGE*>&, std::string FileName);
        
        void formDOTGraph(DotGraph&, const Graph<NODE*,EDGE*>&, uint64_t FormationOptions);

        void buildDotNode(const NODE* N, DotNode* Node);
        void buildDotEdge(const EDGE* Dep, DotEdge* Edge); 
};

} // namespace ppar

#endif // ifndef PPAR_GRAPH_PRINTER_H

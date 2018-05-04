#ifndef PPAR_GRAPH_PASS_H
#define PPAR_GRAPH_PASS_H

#include "ppar_common_includes.h"
#include "Graph.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"

namespace ppar {

template <typename NODE, typename EDGE, typename PASS>
struct GraphPass;

template <typename NODE, typename EDGE, typename PASS>
struct GraphPass<NODE*,EDGE*,PASS> : public llvm::FunctionPass {
    public:
        static char ID;
        GraphPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;
        Graph<NODE*,EDGE*>& getGraph();
        void createGraph(llvm::Function& F) { 
            G = std::make_unique<Graph<NODE*,EDGE*>>(this,F); 
        }

    private:
        std::unique_ptr<Graph<NODE*,EDGE*>> G;
};

} // namespace ppar

#endif // ifndef PPAR_GRAPH_PASS_H

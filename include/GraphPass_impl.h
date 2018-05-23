#ifndef PPAR_GRAPH_PASS_IMPL_H
#define PPAR_GRAPH_PASS_IMPL_H

#include "GraphPass.h"

#define RegisterGraphPass(NODE,EDGE,PASS,VAR) \
\
RegisterPass<ppar::GraphPass<NODE,EDGE,PASS>> \
VAR( PASS::getGraphPassOption(),\
     PASS::getGraphPassOptionDesc() );

namespace ppar {

template <typename NODE, typename EDGE, typename PASS>
GraphPass<NODE*,EDGE*,PASS>::GraphPass() 
 : FunctionPass(ID) {}

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::releaseMemory() {}

template <typename NODE, typename EDGE, typename PASS>
Graph<NODE*,EDGE*>& GraphPass<NODE*,EDGE*,PASS>::getFunctionGraph() { return *G; }

template <typename NODE, typename EDGE, typename PASS>
Graph<NODE*,EDGE*>& GraphPass<NODE*,EDGE*,PASS>::getLoopGraph(const Loop* L) { 
    auto it = LG.find(L);
    if (it != LG.end()) {
        return *(it->second);
    } else {
        return InvalidGraph;
    }
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::allocateGraphs(llvm::Function& F) { 
    const llvm::LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    std::vector<const llvm::Loop*> FunctionLoops;

    if (LI.empty()) {
        return; 
    }
        
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const llvm::Loop* TopLevelL = *loop_it;
        FunctionLoops.push_back(TopLevelL);
        for (auto sub_loop_it = TopLevelL->begin(); sub_loop_it != TopLevelL->end(); ++sub_loop_it) {
            const llvm::Loop* SubL = *sub_loop_it;
            FunctionLoops.push_back(SubL);
        }
    }

    G = std::make_unique<Graph<NODE*,EDGE*>>(this,&F); 
    for (const llvm::Loop* L : FunctionLoops) {
        LG[L] = std::make_unique<Graph<NODE*,EDGE*>>(this,&F); 
    }
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

template <typename NODE, typename EDGE, typename PASS>
llvm::StringRef GraphPass<NODE*,EDGE*,PASS>::getPassName() const { 
    llvm_unreachable("Class template GraphPass cannot be used directly (without concrete specialization)!");
    return "Graph Pass"; 
}

template <typename NODE, typename EDGE, typename PASS>
bool GraphPass<NODE*,EDGE*,PASS>::runOnFunction(llvm::Function& F) {
    llvm_unreachable("Class template GraphPass cannot be used directly (without concrete specialization)!");
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PASS_IMPL_H

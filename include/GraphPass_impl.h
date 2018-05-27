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
Graph<NODE*,EDGE*> GraphPass<NODE*,EDGE*,PASS>::InvalidGraph;

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::releaseMemory() {}

template <typename NODE, typename EDGE, typename PASS>
Graph<NODE*,EDGE*>& GraphPass<NODE*,EDGE*,PASS>::getFunctionGraph() { return *G; }

template <typename NODE, typename EDGE, typename PASS>
Graph<NODE*,EDGE*>& GraphPass<NODE*,EDGE*,PASS>::getLoopGraph(const Loop* L) { 
    auto it = LG.find(L);
    if (it != LG.end()) {
        return *((it->second).get());
    } else {
        return InvalidGraph;
    }
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::allocateGraphs(llvm::Function& F) { 

    // allocate function's dependence graph
    G = std::make_unique<Graph<NODE*,EDGE*>>(this, &F); 

    // if we have got any loops in the function, then proceed with their allocation 
    const llvm::LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    if (LI.empty()) {
        return;
    }
    
    std::vector<const llvm::Loop*> FunctionLoops;
    std::queue<const llvm::Loop*> LoopsQueue;
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const llvm::Loop* TopLevelL = *loop_it;
        LoopsQueue.push(TopLevelL);
        while(!LoopsQueue.empty()) {
            const llvm::Loop* CurrentLoop = LoopsQueue.front();
            FunctionLoops.push_back(CurrentLoop);
            LoopsQueue.pop();
            for (auto sub_loop_it = CurrentLoop->begin(); sub_loop_it != CurrentLoop->end(); ++sub_loop_it) {
                LoopsQueue.push(*sub_loop_it);
            }
        }
    }

    for (const llvm::Loop* L : FunctionLoops) {
        LG[L] = std::make_unique<Graph<NODE*,EDGE*>>(this, &F); 
    }
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

template <typename NODE, typename EDGE, typename PASS>
llvm::StringRef GraphPass<NODE*,EDGE*,PASS>::getPassName() const { 
    return PASS::getPassName(); 
}

template <typename NODE, typename EDGE, typename PASS>
bool GraphPass<NODE*,EDGE*,PASS>::runOnFunction(llvm::Function& F) {
    llvm_unreachable("Class template GraphPass cannot be used directly (without concrete specialization)!");
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PASS_IMPL_H

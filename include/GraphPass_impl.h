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
Graph<NODE*,EDGE*>& GraphPass<NODE*,EDGE*,PASS>::getGraph() { return *G; }

template <typename NODE, typename EDGE, typename PASS>
void GraphPass<NODE*,EDGE*,PASS>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    llvm_unreachable("Class template GraphPass cannot be used directly (without concrete specialization)!");
    AU.setPreservesAll();
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

#include "Graph.h"

namespace ppar {

template <>
llvm::Dependence* mergeEdges<llvm::Instruction*,llvm::Dependence*>(const typename Graph<llvm::Instruction*,llvm::Dependence*>::edge_set& EdgeSet) {
    if (EdgeSet.size() == 1) {
        return (*EdgeSet.begin()).getData();
    } else {
        llvm_unreachable("There can be only one llvm::Dependence* edge in MDG");
        return nullptr;
    }
}

template <>
ppar::Dependence* mergeEdges<llvm::Instruction*,ppar::Dependence*>(const typename Graph<llvm::Instruction*,ppar::Dependence*>::edge_set& EdgeSet) {
    ppar::Dependence* Dep = new ppar::Dependence();
    
    for (const auto& Edge : EdgeSet) {
        Dep->operator+=(*(Edge.getData()));
    }
    
    return Dep;
}

} // namespace ppar

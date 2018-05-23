#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<llvm::DependenceAnalysisWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

template <>
llvm::StringRef GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::getPassName() const {
    return MemoryDependenceGraphPass::getPassName();
}

template <>
bool GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::runOnFunction(Function& F) {

    if (F.isDeclaration()) return false;

    allocateGraphs(F);

    llvm::DependenceInfo& DI = Pass::getAnalysis<llvm::DependenceAnalysisWrapperPass>().getDI();

    // we are only interested in memory referencing instructions
    std::vector<Instruction*> MemRefs;
    for (llvm::inst_iterator I = llvm::inst_begin(F), E = llvm::inst_end(F); I != E; ++I) {
        if (isa<StoreInst>(*I) || isa<LoadInst>(*I)) {
            MemRefs.push_back(&*I);
        }
    }

    /* Build Memory Dependence Graph for the given function F */

    for (auto Inst : MemRefs) {
        getFunctionGraph().addNode(Inst);
    }
    
    for (auto SrcI : MemRefs) {
        for (auto DstI : MemRefs) {
            if (llvm::Dependence* D = (DI.depends(SrcI, DstI, true)).release()) {
                if (D->isFlow() || D->isAnti()) getFunctionGraph().addEdge(SrcI, DstI, D);
            }
        }
    }
    
    /* Build Memory Dependence Graphs for all loops of the given function F */
    
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    if (LI.empty()) {
        return false; 
    }
        
    // add nodes
    for (auto Inst : MemRefs) {
        Loop* InnermostL = LI.getLoopFor(Inst->getParent());
        if (InnermostL != nullptr) {
            getLoopGraph(InnermostL).addNode(Inst);
        }
    }
    // add edges
    for (auto loop_it = LG.begin(); loop_it != LG.end(); ++loop_it) {
        const llvm::Loop* L = loop_it->first;
        Graph<llvm::Instruction*,llvm::Dependence*>* G = (loop_it->second).get();
        
        for (typename Graph<llvm::Instruction*,llvm::Dependence*>::const_node_iterator src_node_it = G->nodes_cbegin(); 
             src_node_it != G->nodes_cend(); src_node_it++) {
            GraphNode<llvm::Instruction*,llvm::Dependence*> DepNode = *src_node_it;
            const llvm::Instruction* SrcI = DepNode.getNode();
            
            for (typename Graph<llvm::Instruction*,llvm::Dependence*>::const_node_iterator dst_node_it = G->nodes_cbegin(); 
                 dst_node_it != G->nodes_cend(); dst_node_it++) {
                GraphNode<llvm::Instruction*,llvm::Dependence*> DepNode = *dst_node_it;
                const llvm::Instruction* DstI = DepNode.getNode();
           
                if (llvm::Dependence* D = (DI.depends(const_cast<llvm::Instruction*>(SrcI), const_cast<llvm::Instruction*>(DstI), true)).release()) {
                    if (D->isFlow() || D->isAnti()) G->addEdge(SrcI, DstI, D);
                }
            }
        }
    }
    
    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H

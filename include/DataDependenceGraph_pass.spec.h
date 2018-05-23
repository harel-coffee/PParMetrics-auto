#ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

template <>
llvm::StringRef GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::getPassName() const {
    return DataDependenceGraphPass::getPassName();
}

template <>
bool GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::runOnFunction(Function& F) {

    if (F.isDeclaration()) return false;
    
    allocateGraphs(F);

    /* Build Data Dependence Graph for the given function F */

    // add nodes
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
        Instruction* Inst = &*I;
        getFunctionGraph().addNode(Inst);
    }
    // add edges
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        // every user of an instruction is its user
        for (User* U : I->users()) {
            ppar::Dependence* Dep = new ppar::Dependence();
            Dep->setData();
            Dep->setFlow();
            Dep->setReg();

            if (Instruction* Inst = dyn_cast<Instruction>(U)) {
                getFunctionGraph().addEdge(&*I, Inst, Dep);
            }
        }
    }
    
    /* Build Data Dependence Graphs for all loops of the given function F */

    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    if (LI.empty()) {
        return false; 
    }
        
    // add nodes
    for (Function::iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        Loop* InnermostL = LI.getLoopFor(&(*bb_it));
        if (InnermostL != nullptr) {
            for (BasicBlock::iterator inst_it = bb_it->begin(); inst_it != bb_it->end(); ++inst_it) {
                Instruction* Inst = &*inst_it;
                getLoopGraph(InnermostL).addNode(Inst);
            }
        } 
    }
    // add edges
    for (auto loop_it = LG.begin(); loop_it != LG.end(); ++loop_it) {
        const llvm::Loop* L = loop_it->first;
        Graph<llvm::Instruction*,ppar::Dependence*>* G = (loop_it->second).get();
        for (typename Graph<llvm::Instruction*,ppar::Dependence*>::const_node_iterator node_it = G->nodes_cbegin(); 
             node_it != G->nodes_cend(); node_it++) {
            GraphNode<llvm::Instruction*,ppar::Dependence*> DepNode = *node_it;
            const llvm::Instruction* I = DepNode.getNode();
            // every user of an instruction is its user
            for (const User* U : I->users()) {
                ppar::Dependence* Dep = new ppar::Dependence();
                Dep->setData();
                Dep->setFlow();
                Dep->setReg();

                if (const Instruction* Inst = dyn_cast<const Instruction>(U)) {
                    G->addEdge(&*I, Inst, Dep);
                }
            }
        }
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

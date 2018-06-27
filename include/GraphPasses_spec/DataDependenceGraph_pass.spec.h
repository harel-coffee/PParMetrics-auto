#ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
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
    
    ppar::FunctionLoopInfoPass& LInfoPass = (getAnalysis<FunctionLoopInfoPass>());
    const FunctionLoopInfoPass::FunctionLoopList* LList = LInfoPass.getFunctionLoopList(&F);
    if (LList->empty()) {
        // no loops -> no work to do
        return false;
    }
    // allocate dependence graphs for function loops
    for (const llvm::Loop* L : *LList) {
        Graph<llvm::Instruction*,ppar::Dependence*>& LG = getLoopGraph(L);
        if (LG == InvalidGraph) {
            // all loop graphs must have been allocated in allocateGraphs by that point
            llvm_unreachable("llvm::Loop cannot have InvalidGraph allocated to it!");
        } else {
            // add nodes to the loop graph
            for (typename Loop::block_iterator bb_it = L->block_begin(); bb_it != L->block_end(); ++bb_it) {
                for (typename BasicBlock::iterator inst_it = (*bb_it)->begin(); inst_it != (*bb_it)->end(); ++inst_it) {
                    Instruction* Inst = &(*inst_it);
                    LG.addNode(Inst);
                }
            }
            // add edges to the loop graph
            for (typename Graph<llvm::Instruction*,ppar::Dependence*>::const_node_iterator node_it = LG.nodes_cbegin(); 
                 node_it != LG.nodes_cend(); node_it++) {
                GraphNode<llvm::Instruction*,ppar::Dependence*> DepNode = *node_it;
                const llvm::Instruction* I = DepNode.getNode();
                // every user of an instruction is its user
                for (const User* U : I->users()) {
                    if (const Instruction* UserInst = dyn_cast<const Instruction>(U)) {
                        llvm::Loop* UserL = LI.getLoopFor(UserInst->getParent());
                        if (UserL == L) {
                            ppar::Dependence* Dep = new ppar::Dependence();
                            Dep->setData();
                            Dep->setFlow();
                            Dep->setReg();

                            LG.addEdge(&*I, UserInst, Dep);
                        }
                    }
                }
            }
        }
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

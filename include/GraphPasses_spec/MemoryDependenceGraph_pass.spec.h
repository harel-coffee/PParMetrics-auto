#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<llvm::DependenceAnalysisWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
}

template <>
bool GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::runOnFunction(Function& F) {

    if (F.isDeclaration()) return false;

    allocateGraphs(F);

    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        dbgs() << "GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::runOnFunction(" << F.getName() << ")\n";
    );

    llvm::DependenceInfo& DI = Pass::getAnalysis<llvm::DependenceAnalysisWrapperPass>().getDI();

    // we are only interested in memory referencing instructions
    std::vector<Instruction*> MemRefs;
    for (llvm::inst_iterator I = llvm::inst_begin(F), E = llvm::inst_end(F); I != E; ++I) {
        if (isa<StoreInst>(*I) || isa<LoadInst>(*I)) {
            MemRefs.push_back(&*I);
        }
    }

    /* Build Memory Dependence Graph (MDG) for the given function F */

    for (auto Inst : MemRefs) {
        getFunctionGraph().addNode(Inst);
    }

    for (std::vector<Instruction*>::iterator src_it = MemRefs.begin(); src_it != MemRefs.end(); ++src_it) {
        for (std::vector<Instruction*>::iterator dst_it = src_it; dst_it != MemRefs.end(); ++dst_it) {
            if (llvm::Dependence* D = (DI.depends(*src_it, *dst_it, true)).release()) {
                if (D->isFlow() || D->isAnti() || D->isOutput()) {
                    getFunctionGraph().addEdge(*src_it, *dst_it, D);
                }
            }
        }
    }
    
    /* Build Memory Dependence Graphs (MDG) for all loops of the given function F */
    
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    if (LI.empty()) { // no loops in the given function
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
        Graph<llvm::Instruction*,llvm::Dependence*>& LG = getLoopGraph(L);
        if (LG == InvalidGraph) {
            // all loop graphs must have been allocated in allocateGraphs by that point
            llvm_unreachable("llvm::Loop cannot have InvalidGraph allocated to it!");
        } else {
            // add nodes to the loop graph
            for (typename Loop::block_iterator bb_it = L->block_begin(); bb_it != L->block_end(); ++bb_it) {
                for (typename BasicBlock::iterator inst_it = (*bb_it)->begin(); inst_it != (*bb_it)->end(); ++inst_it) {
                    if (isa<StoreInst>(*inst_it) || isa<LoadInst>(*inst_it)) {
                        Instruction* Inst = &(*inst_it);
                        LG.addNode(Inst);
                    }
                }
            }
            // add edges to the loop graph
            for (typename Graph<llvm::Instruction*,llvm::Dependence*>::const_node_iterator src_node_it = LG.nodes_cbegin(); 
                 src_node_it != LG.nodes_cend(); src_node_it++) {
                GraphNode<llvm::Instruction*,llvm::Dependence*> DepNode = *src_node_it;
                const llvm::Instruction* SrcI = DepNode.getNode();
                for (typename Graph<llvm::Instruction*,llvm::Dependence*>::const_node_iterator dst_node_it = src_node_it; 
                     dst_node_it != LG.nodes_cend(); dst_node_it++) {
                    GraphNode<llvm::Instruction*,llvm::Dependence*> DepNode = *dst_node_it;
                    const llvm::Instruction* DstI = DepNode.getNode();
                    if (llvm::Dependence* D = (DI.depends(const_cast<llvm::Instruction*>(SrcI), const_cast<llvm::Instruction*>(DstI), true)).release()) {
                        bool addE = false;

                        if (D->isConfused()) {
                            addE = true;
                        } 
                        
//                        bool LoopIndep = D->isLoopIndependent();
                        
//                        if (!LoopIndep) {
//                            addE = true;
//                        }

                        uint64_t dir = D->getDirection(L->getLoopDepth());
                        if ( (dir != llvm::Dependence::DVEntry::EQ) && 
                             (dir != llvm::Dependence::DVEntry::NONE) && 
                             (dir != llvm::Dependence::DVEntry::ALL) ) {
                            addE = true;
                        }

                        if (D->isOutput()) {
                            addE = true;
                        }

                        if (D->isScalar(L->getLoopDepth())) {
                            addE = true;
                        }

                        if (addE) {
                            if (D->isFlow() || D->isAnti() || D->isOutput()) {
                                LG.addEdge(SrcI, DstI, D);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PASS_SPEC_H

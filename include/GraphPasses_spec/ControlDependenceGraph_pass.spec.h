#ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

// Depth-first search the post-dominator tree in order to construct a top-down
// traversal
stack<const DomTreeNode*> getBottomUpTraversal(const PostDominatorTree& pdt) {
    stack<const DomTreeNode*> Parents;
    stack<const DomTreeNode*> Result;

    Parents.push(pdt.getRootNode());
    while (!Parents.empty()) {
        const DomTreeNode* Current = Parents.top();
        Parents.pop();
        if (Current->getBlock() != nullptr) { // getBlock() might be null if there are multiple exits from F
            Result.push(Current);
        }
        for (DomTreeNode::const_iterator it = Current->begin(); it != Current->end(); ++it) {
            Parents.push(*it);
        }
    }

    return Result;
}

// Get the post-dominator frontier given a post-dominator tree and a bottom-up
// traversal for it
map<const BasicBlock*, set<const BasicBlock*>> 
getPostDomFrontier(const PostDominatorTree& pdt,
                   stack<const DomTreeNode*>&& bottom_up_traversal) {
    map<const BasicBlock*, set<const BasicBlock*>> Result;

    while (!bottom_up_traversal.empty()) {
        const DomTreeNode* Current = bottom_up_traversal.top();
        bottom_up_traversal.pop();

        BasicBlock* CurrentBB = Current->getBlock();
        Result[CurrentBB];

        // For each predecessor of `currentBB` in the CFG
        for (pred_iterator it = pred_begin(CurrentBB); it != pred_end(CurrentBB); ++it) {
            // If it is not immediately post-dominated by `currentBB`, then it is in
            // the post-dominance frontier of `currentBB`
            if (pdt.getNode(*it)->getIDom() != Current) {
                Result[CurrentBB].insert(*it);
            }
        }

        // for each node post-dominated by 'current' (i.e. for each 'child' node of
        // 'current' in a post-dominator tree)
        for (DomTreeNode::const_iterator it = Current->begin(); it != Current->end(); ++it) {
            const BasicBlock* ChildBlock = (*it)->getBlock(); // Z in the algorithm
            for (const BasicBlock* BB : Result[ChildBlock]) {
                if (pdt.getNode(const_cast<BasicBlock*>(BB))->getIDom() != Current) {
                    Result[CurrentBB].insert(BB);
                }
            }
        }
    }

    return Result;
}

template <>
bool GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::runOnFunction(Function& F) {
    
    if (F.isDeclaration()) return false;

    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        dbgs() << "GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::runOnFunction(" << F.getName() << ")\n";
    );

    allocateGraphs(F);

    const PostDominatorTree& pdt = Pass::getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    stack<const DomTreeNode*> bottom_up_traversal = getBottomUpTraversal(pdt);
    map<const BasicBlock*, set<const BasicBlock*>> post_dom_frontier = getPostDomFrontier(pdt, std::move(bottom_up_traversal));

    /* Build Control Dependence Graph for the given function F */

    // Reverse the post_dom_frontier map and store as a graph.
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* Node = kv.first;
        getFunctionGraph().addNode(const_cast<BasicBlock*>(Node));
    }
  
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* To = kv.first;
        for (const BasicBlock* From : kv.second) {
            ppar::Dependence* Dep = new ppar::Dependence();
            Dep->setControl();
            getFunctionGraph().addEdge(const_cast<BasicBlock*>(From), 
                        const_cast<BasicBlock*>(To), 
                        Dep);
        }
    }

    /* Build Control Dependence Graphs for all loops of the given function F */

    ppar::FunctionLoopInfoPass& LInfoPass = (getAnalysis<FunctionLoopInfoPass>());
    const FunctionLoopInfoPass::FunctionLoopList* LList = LInfoPass.getFunctionLoopList(&F);
    if (LList->empty()) {
        // no loops -> no work to do
        return false;
    }
    // allocate dependence graphs for function loops
    for (const llvm::Loop* L : *LList) {
        Graph<llvm::BasicBlock*,ppar::Dependence*>& LG = getLoopGraph(L);

        if (LG == InvalidGraph) {
            // all loop graphs must have been allocated in allocateGraphs by that point
            llvm_unreachable("llvm::Loop cannot have InvalidGraph allocated to it!");
        } else {
            // add all basic blocks of the loop to the graph
            for (typename Loop::block_iterator bb_it = L->block_begin(); bb_it != L->block_end(); ++bb_it) {
                LG.addNode(*bb_it);
            }
        }

        // add edges to the loop graph
        for (typename Graph<llvm::BasicBlock*,ppar::Dependence*>::const_node_iterator node_it = LG.nodes_cbegin(); 
                 node_it != LG.nodes_cend(); node_it++) {
            GraphNode<llvm::BasicBlock*,ppar::Dependence*> DepNode = *node_it;
            const llvm::BasicBlock* To = DepNode.getNode();
            std::set<const llvm::BasicBlock*>& pDomFrontier = post_dom_frontier[To];
 
            for (const llvm::BasicBlock* From : pDomFrontier) {
                if (LG.getNode(From) != Graph<llvm::BasicBlock*,ppar::Dependence*>::InvalidNode) {
                    ppar::Dependence* Dep = new ppar::Dependence();
                    Dep->setControl();
                    LG.addEdge(const_cast<BasicBlock*>(From), 
                               const_cast<BasicBlock*>(To), 
                               Dep);
                }
            }                
        }
    }

    /*
    // Reverse the post_dom_frontier map and store as a graph.
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* Node = kv.first;
        Loop* InnermostL = LI.getLoopFor(Node);
        if (InnermostL != nullptr) { // BB is inside the loop
            Graph<llvm::BasicBlock*,ppar::Dependence*>& LG = getLoopGraph(InnermostL);
            if (LG != InvalidGraph) {
                LG.addNode(const_cast<BasicBlock*>(Node));
            } else {
                llvm_unreachable("llvm::Loop cannot have InvalidGraph allocated to it!");
            }
        }
    }
  
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* To = kv.first;
        for (const BasicBlock* From : kv.second) {
            Loop* FromL = LI.getLoopFor(From);
            Loop* ToL = LI.getLoopFor(To);
            
            if ((FromL != nullptr) &&
                (ToL != nullptr) &&
                (FromL == ToL)) {
                ppar::Dependence* Dep = new ppar::Dependence();
                Dep->setControl();
                getLoopGraph(ToL).addEdge(const_cast<BasicBlock*>(From), 
                                          const_cast<BasicBlock*>(To), 
                                          Dep);
            }
        }
    }*/

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H

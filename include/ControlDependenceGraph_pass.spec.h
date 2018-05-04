#ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
}

template <>
llvm::StringRef GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>::getPassName() const {
    return ControlDependenceGraphPass::getPassName();
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
   
    createGraph(F); 

    const PostDominatorTree& pdt = Pass::getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    stack<const DomTreeNode*> bottom_up_traversal = getBottomUpTraversal(pdt);
    map<const BasicBlock*, set<const BasicBlock*>> post_dom_frontier = getPostDomFrontier(pdt, std::move(bottom_up_traversal));

    // Reverse the post_dom_frontier map and store as a graph.
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* Node = kv.first;
        getGraph().addNode(const_cast<BasicBlock*>(Node));
    }
  
    for (auto& kv : post_dom_frontier) {
        const BasicBlock* To = kv.first;
        for (const BasicBlock* From : kv.second) {
            ppar::Dependence* Dep = new ppar::Dependence();
            Dep->setControl();
            getGraph().addEdge(const_cast<BasicBlock*>(From), 
                        const_cast<BasicBlock*>(To), 
                        Dep);
        }
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_CONTROL_DEPENDENCE_GRAPH_PASS_SPEC_H

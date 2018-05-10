#ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_PROGRAM_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template <>
char GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>>();
    AU.addRequired<GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>>();
}

template <>
llvm::StringRef GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::getPassName() const {
    return ProgramDependenceGraphPass::getPassName();
}

template <>
bool GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::runOnFunction(Function& F) {

    if (F.isDeclaration()) return false;

    createGraph(F);

    const Graph<Instruction*,llvm::Dependence*>& mdg =
        Pass::getAnalysis<GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>>().getGraph();
    const Graph<Instruction*,ppar::Dependence*>& ddg =
        Pass::getAnalysis<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>>().getGraph();
    const Graph<BasicBlock*,ppar::Dependence*>& cdg =
        Pass::getAnalysis<GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>>().getGraph();

    // data dependence graph consists of all program's instructions ->
    // -> add them to program dependence graph as nodes
    for (auto node_it = ddg.nodes_cbegin(); node_it != ddg.nodes_cend(); node_it++) {
        const GraphNode<Instruction*,ppar::Dependence*>& Node = *node_it; 
        getGraph().addNode(Node.getNode());
    }

    // copy all DDG edges to the PDG
    for (auto edge_it = ddg.edges_cbegin(); edge_it != ddg.edges_cend(); edge_it++) {
        const std::pair<const Instruction*,const Instruction*> NodePair = edge_it->first;
        const Graph<Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;

        for (const auto& Edge : EdgeSet) {
            ppar::Dependence* Dep = new ppar::Dependence();
            *Dep = *(Edge.getData());
            getGraph().addEdge(Edge.getFrom(), Edge.getTo(), Dep);
        }
    }

    // copy MDG edges to the PDG
    for (auto edge_it = mdg.edges_cbegin(); edge_it != mdg.edges_cend(); edge_it++) {
        const std::pair<const Instruction*,const Instruction*> NodePair = edge_it->first;
        const Graph<Instruction*,llvm::Dependence*>::edge_set& EdgeSet = edge_it->second;
        for (const auto& Edge : EdgeSet) {
            ppar::Dependence* Dep(new ppar::Dependence());
            llvm::Dependence* MemDep = Edge.getData();

            Dep->setData();
            if (MemDep->isFlow()) {
                Dep->setFlow();
            } else if (MemDep->isAnti()) {
                Dep->setAnti();
            } else if (MemDep->isOutput()){
                Dep->setOutput();
            } else {
                Dep->setUnknown();
            }
            Dep->setMem();

            if (MemDep->isConfused()) {
                Dep->Confused = true;
            } else {
                Dep->Confused = false;
                llvm::FullDependence* FullDep = static_cast<llvm::FullDependence*>(MemDep);
                Dep->LoopIndependent = FullDep->isLoopIndependent();
            }
            Dep->Consistent = MemDep->isConsistent();
            
            getGraph().addEdge(Edge.getFrom(), Edge.getTo(), Dep);
        }
    }

    // For each dependence in the CDG: construct a dependence from the last
    // instruction of the source basic block to the each instruction in the
    // target basic block
    for (auto edge_it = cdg.edges_cbegin(); edge_it != cdg.edges_cend(); ++edge_it) {
        const std::pair<const BasicBlock*,const BasicBlock*> NodePair = edge_it->first;
        const Graph<BasicBlock*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
        for (const auto& Edge : EdgeSet) {
            const Instruction& source = Edge.getFrom()->back();
            for (BasicBlock::const_iterator it = Edge.getTo()->begin(); it != Edge.getTo()->end(); ++it) {
                ppar::Dependence* Dep(new ppar::Dependence());
                Dep->setControl();
                getGraph().addEdge(const_cast<Instruction*>(&source), const_cast<Instruction*>(&*it), Dep);
            } 
        }
    }

    return false;


}

} // namespace ppar

#endif // #ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PASS_SPEC_H

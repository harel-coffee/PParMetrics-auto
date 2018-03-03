#include "ProgramDependenceGraph.h"
#include "MemoryDependenceGraph.h"
#include "DataDependenceGraph.h"

#include "llvm/PassSupport.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/User.h"
#include "llvm/Support/Casting.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/DependenceAnalysis.h"
using namespace llvm;

#include <vector>
using namespace std;

char ppar::ProgramDependenceGraphPass::ID = 0;
RegisterPass<ppar::ProgramDependenceGraphPass> PDGRegister("pdg", "Build Program Dependence Graph");

namespace ppar {

void ProgramDependenceGraphPass::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<MemoryDependenceGraphPass>();
    AU.addRequired<DataDependenceGraphPass>();
}

bool ProgramDependenceGraphPass::runOnFunction(Function& F) {
    const DependenceGraph<Instruction*,llvm::Dependence*>& mdg = Pass::getAnalysis<MemoryDependenceGraphPass>().getMDG();
    const DependenceGraph<Instruction*,ppar::Dependence*>& ddg = Pass::getAnalysis<DataDependenceGraphPass>().getDDG();

    // data dependence graph consists of all program's instructions ->
    // -> add them to program dependence graph as nodes
    for (auto node_it = ddg.nodes_cbegin(); node_it != ddg.nodes_cend(); node_it++) {
        const DependenceGraphNode<Instruction*>& Node = *node_it; 
        PDG.addNode(Node.getNode());
    }

    // copy all DDG edges to the PDG
    for (auto edge_it = ddg.edges_cbegin(); edge_it != ddg.edges_cend(); edge_it++) {
        const DependenceGraphEdge<Instruction*,ppar::Dependence*>& Edge = *edge_it;
        PDG.addEdge(Edge.getFrom(), Edge.getTo(), Edge.getData());
    }

    // copy MDG edges to the PDG
    for (auto edge_it = mdg.edges_cbegin(); edge_it != mdg.edges_cend(); edge_it++) {
        const DependenceGraphEdge<Instruction*,llvm::Dependence*>& Edge = *edge_it;
        ppar::Dependence* Dep = new ppar::Dependence();
        llvm::Dependence* MemDep = Edge.getData();
        
        if (MemDep->isFlow()) {
            Dep->Flow = true;
        } else if (MemDep->isAnti()) {
            Dep->Anti = true;
        } else if (MemDep->isOutput()){
            Dep->Output = true;
        }

        PDG.addEdge(Edge.getFrom(), Edge.getTo(), Dep);
    }

    return false;
}

}

#include "ProgramDependenceGraph.h"
#include "MemoryDependenceGraph.h"
#include "DataDependenceGraph.h"
#include "ControlDependenceGraph.h"

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
#include <memory>
using namespace std;

char ppar::ProgramDependenceGraphPass::ID = 0;
RegisterPass<ppar::ProgramDependenceGraphPass> PDGRegister("pdg", "Build Program Dependence Graph");

namespace ppar {

void ProgramDependenceGraphPass::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<DependenceAnalysisWrapperPass>();
    AU.addRequired<DataDependenceGraphPass>();
    AU.addRequired<MemoryDependenceGraphPass>();
    AU.addRequired<ControlDependenceGraphPass>();
}

bool ProgramDependenceGraphPass::runOnFunction(Function& F) {
    const DependenceGraph<Instruction*,llvm::Dependence*>& mdg = Pass::getAnalysis<MemoryDependenceGraphPass>().getMDG();
    const DependenceGraph<Instruction*,ppar::Dependence*>& ddg = Pass::getAnalysis<DataDependenceGraphPass>().getDDG();
    const DependenceGraph<BasicBlock*,ppar::Dependence*>& cdg = Pass::getAnalysis<ControlDependenceGraphPass>().getCDG();

    // data dependence graph consists of all program's instructions ->
    // -> add them to program dependence graph as nodes
    for (auto node_it = ddg.nodes_cbegin(); node_it != ddg.nodes_cend(); node_it++) {
        const DependenceGraphNode<Instruction*,ppar::Dependence*>& Node = *node_it; 
        PDG.addNode(Node.getNode());
    }

    // copy all DDG edges to the PDG
    for (auto edge_it = ddg.edges_cbegin(); edge_it != ddg.edges_cend(); edge_it++) {
        const std::pair<Instruction*,Instruction*> NodePair = edge_it->first;
        const DependenceGraph<Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;

        for (const auto& Edge : EdgeSet) {
            ppar::Dependence* Dep = new ppar::Dependence();
            *Dep = *(Edge.getData());
            PDG.addEdge(Edge.getFrom(), Edge.getTo(), Dep);
        }
    }

    // copy MDG edges to the PDG
    for (auto edge_it = mdg.edges_cbegin(); edge_it != mdg.edges_cend(); edge_it++) {
        const std::pair<Instruction*,Instruction*> NodePair = edge_it->first;
        const DependenceGraph<Instruction*,llvm::Dependence*>::edge_set& EdgeSet = edge_it->second;
        for (const auto& Edge : EdgeSet) {
            ppar::Dependence* Dep(new ppar::Dependence());
            llvm::Dependence* MemDep = Edge.getData();

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
            }

            if (MemDep->isConsistent()) {
                Dep->Consistent = true;
            } else {
                Dep->Consistent = false;
            }
            
            PDG.addEdge(Edge.getFrom(), Edge.getTo(), Dep);
        }
    }

    // For each dependence in the CDG: construct a dependence from the last
    // instruction of the source basic block to the each instruction in the
    // target basic block
    for (auto edge_it = cdg.edges_cbegin(); edge_it != cdg.edges_cend(); ++edge_it) {
        const std::pair<BasicBlock*,BasicBlock*> NodePair = edge_it->first;
        const DependenceGraph<BasicBlock*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
        for (const auto& Edge : EdgeSet) {
            const Instruction& source = Edge.getFrom()->back();
            for (BasicBlock::const_iterator it = Edge.getTo()->begin(); it != Edge.getTo()->end(); ++it) {
                ppar::Dependence* Dep(new ppar::Dependence());
                Dep->setControl();
                PDG.addEdge(const_cast<Instruction*>(&source), const_cast<Instruction*>(&*it), Dep);
            } 
        }
    }

    return false;
}

StringRef ProgramDependenceGraphPass::getPassName() const { 
    return "Program Dependence Graph"; 
}

void ProgramDependenceGraphPass::releaseMemory() {
}

const DependenceGraph<llvm::Instruction*,ppar::Dependence*>& ProgramDependenceGraphPass::getPDG() const { 
    return PDG; 
}

}

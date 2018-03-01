#include "MDGPrinter.h"

#include <map>
using namespace std;

#include "llvm/Support/raw_ostream.h"

namespace {

bool MDGPrinter::runOnFunction(Function &F) {
    MemoryDependenceGraphPass &mdg = Pass::getAnalysis<MemoryDependenceGraphPass>();
    DependenceGraph<Instruction*,Dependence*>& DG = mdg.getMDG();
    map<Instruction*,int> instr_to_nodenum;

    // print DOT graph header
    outs() << "digraph {\n\n";
    
    // print all graph nodes
    int node_num = 0;
    instr_to_nodenum.clear();
    for(DependenceGraph<Instruction*,Dependence*>::nodes_iterator node_it = DG.nodes_begin(); node_it != DG.nodes_end(); node_it++) {
        DependenceGraphNode<Instruction*> node = *node_it;
        Instruction* instr = node.getNode();
        instr_to_nodenum[instr] = node_num;
        outs() << "node" << node_num++ << "\n";
    }
    outs() << "\n";

    // print all graph edges
    for(DependenceGraph<Instruction*,Dependence*>::edges_iterator edge_it = DG.edges_begin(); edge_it != DG.edges_end(); edge_it++) {
        DependenceGraphEdge<Instruction*,Dependence*> edge = *edge_it;
        Instruction* from = edge.getFrom();
        Instruction* to = edge.getTo();
        Dependence* data = edge.getData();
        outs() << "node" << instr_to_nodenum[from] << "->" << "node" << instr_to_nodenum[to] << "\n";
    }
    outs() << "\n";
    // close DOT graph header
    outs() << "}";

    return false;
}

void MDGPrinter::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<MemoryDependenceGraphPass>();
}

} // anonymous namespace

char MDGPrinter::ID = 0;
RegisterPass<MDGPrinter> MDGPrinterRegister("dot-mdg", "Print MDG of a function to 'dot' file");

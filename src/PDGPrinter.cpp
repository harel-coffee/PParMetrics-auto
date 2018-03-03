#include "PDGPrinter.h"
#include "DotPrinter.h"

#include <string>
using namespace std;

#include "llvm/PassRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

char ppar::PDGPrinter::ID = 0;
RegisterPass<ppar::PDGPrinter> PDGPrinterRegister("dot-pdg", "Print PDG of the function to the 'dot' file");

namespace ppar {

PDGPrinter::PDGPrinter() 
    : FunctionPass(ID) {}

void PDGPrinter::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<ProgramDependenceGraphPass>();
}

bool PDGPrinter::runOnFunction(Function& F) {
    ProgramDependenceGraphPass& pdg = Pass::getAnalysis<ProgramDependenceGraphPass>();
    const DependenceGraph<Instruction*,ppar::Dependence*>& DG = pdg.getPDG();
    DotPrinter Printer;
    map<Instruction*,string> InstrToNodeName;
    
    for(DependenceGraph<Instruction*,ppar::Dependence*>::const_nodes_iterator node_it = DG.nodes_cbegin(); node_it != DG.nodes_cend(); node_it++) {
        DependenceGraphNode<Instruction*> DepNode = *node_it;
        Instruction* Instr = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[Instr] = Node->getName();

        if (Instr->mayReadOrWriteMemory()) {
            // memory reference
            Node->setAttribute( /* name = */ string("shape"), /* value = */ string("ellipse"));
        } else {
            Node->setAttribute( /* name = */ string("shape"), /* value = */ string("rectangle"));
        }

        string str;
        raw_string_ostream rso(str);
        Instr->print(rso);
        Node->setAttribute( /* name = */ string("label"), /* value = */ str);

        Printer.addNode(Node->getName(), Node);
    }

    // print all graph edges
    for(DependenceGraph<Instruction*,ppar::Dependence*>::const_edges_iterator edge_it = DG.edges_cbegin(); edge_it != DG.edges_cend(); edge_it++) {
        DependenceGraphEdge<Instruction*,ppar::Dependence*> DepEdge = *edge_it;
        Instruction* From = DepEdge.getFrom();
        Instruction* To = DepEdge.getTo();
        ppar::Dependence* Data = DepEdge.getData();
        string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
        DotEdge* Edge = new DotEdge(EdgeName);

        Printer.addEdge(Edge->getName(), Edge);
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

} // namespace ppar

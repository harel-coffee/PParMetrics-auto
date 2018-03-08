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
    DotPrinter Printer("pdg");
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
        ppar::Dependence* Dep = DepEdge.getData();
        string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
        DotEdge* Edge = new DotEdge(EdgeName);

        if (Dep->isFlow()) {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("T"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("forestgreen"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("forestgreen"));
        } else if (Dep->isAnti()) {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("A"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("firebrick3"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("firebrick3"));
        } else if (Dep->isOutput())   {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("O"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("dodgerblue"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("dodgerblue"));
        } else {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("U"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("gray"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("gray"));
        }

        Printer.addEdge(Edge->getName(), Edge);
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

} // namespace ppar

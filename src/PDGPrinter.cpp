#include "PDGPrinter.h"
#include "DotPrinter.h"

#include <string>
#include <algorithm>
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
    DotPrinter Printer(F.getName().str() + ".pdg");
    map<Instruction*,string> InstrToNodeName;
    
    for(DependenceGraph<Instruction*,ppar::Dependence*>::const_nodes_iterator node_it = DG.nodes_cbegin(); node_it != DG.nodes_cend(); node_it++) {
        DependenceGraphNode<Instruction*> DepNode = *node_it;
        Instruction* Instr = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[Instr] = Node->getName();

        if (Instr->mayReadOrWriteMemory()) {
            // memory reference
            Node->setAttribute( /* name = */ string("shape"), /* value = */ string("ellipse"));
            Node->setAttribute( /* name = */ string("style"), /* value = */ string("filled"));
            Node->setAttribute( /* name = */ string("fillcolor"), /* value = */ string("lightcoral"));
        } else {
            Node->setAttribute( /* name = */ string("shape"), /* value = */ string("rectangle"));
            Node->setAttribute( /* name = */ string("style"), /* value = */ string("filled"));
            Node->setAttribute( /* name = */ string("fillcolor"), /* value = */ string("lemonchiffon"));
        }

        string str;
        raw_string_ostream rso(str);
        Instr->print(rso);
        std::remove(str.begin(), str.end(), '\"');
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
        
        if (Dep->isMem()) {
            Edge->setAttribute( /* name = */ string("style"), /* value = */ string("solid"));
        } else {
            Edge->setAttribute( /* name = */ string("style"), /* value = */ string("dotted"));
        }
        
        if (Dep->isConfused()) {
            if (!Dep->isConsistent()) {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("1.0"));
            } else {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("0.0"));
            }
        } else {
            if (Dep->isConsistent()) {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("2.0"));
            } else {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("0.0"));
            }
        }

        Printer.addEdge(Edge->getName(), Edge);
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

} // namespace ppar

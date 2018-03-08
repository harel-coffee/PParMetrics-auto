#include "MDGPrinter.h"
#include "DotPrinter.h"

#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include <string>
#include <map>
#include <memory>
using namespace std;

char ppar::MDGPrinter::ID = 0;
RegisterPass<ppar::MDGPrinter> MDGPrinterRegister("dot-mdg", "Print MDG of a function to 'dot' file");

namespace ppar {

MDGPrinter::MDGPrinter() 
    : FunctionPass(ID) {}

void MDGPrinter::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<MemoryDependenceGraphPass>();
}

bool MDGPrinter::runOnFunction(Function &F) {
    MemoryDependenceGraphPass& MDG = Pass::getAnalysis<MemoryDependenceGraphPass>();
    DependenceGraph<Instruction*,llvm::Dependence*>& DG = MDG.getMDG();
    DotPrinter Printer("mdg");
    map<Instruction*,string> InstrToNodeName;
    
    for(DependenceGraph<Instruction*,llvm::Dependence*>::nodes_iterator node_it = DG.nodes_begin(); node_it != DG.nodes_end(); node_it++) {
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
    for(DependenceGraph<Instruction*,llvm::Dependence*>::edges_iterator edge_it = DG.edges_begin(); edge_it != DG.edges_end(); edge_it++) {
        const DependenceGraphEdge<Instruction*,llvm::Dependence*>& DepEdge = *edge_it;
        Instruction* From = DepEdge.getFrom();
        Instruction* To = DepEdge.getTo();
        llvm::Dependence* Data = DepEdge.getData();
        string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
        DotEdge* Edge = new DotEdge(EdgeName);

        Printer.addEdge(Edge->getName(), Edge);
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

} // namespace ppar

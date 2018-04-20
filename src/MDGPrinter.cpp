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
    Graph<Instruction*,llvm::Dependence*>& DG = MDG.getMDG();
    DotPrinter Printer("mdg");
    map<Instruction*,string> InstrToNodeName;
    
    for(Graph<Instruction*,llvm::Dependence*>::node_iterator node_it = DG.nodes_begin(); node_it != DG.nodes_end(); node_it++) {
        GraphNode<Instruction*,llvm::Dependence*> DepNode = *node_it;
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

        Printer.getGraph().addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = DG.edges_cbegin(); edge_it != DG.edges_cend(); edge_it++) {
        const std::pair<Instruction*,Instruction*>& NodePair = edge_it->first;
        const Graph<Instruction*,llvm::Dependence*>::edge_set& EdgeSet = edge_it->second;

        for (const auto& DepEdge : EdgeSet) {
            Instruction* From = DepEdge.getFrom();
            Instruction* To = DepEdge.getTo();
            llvm::Dependence* Data = DepEdge.getData();
            string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
            DotEdge* Edge = new DotEdge(EdgeName);

            Printer.getGraph().addEdge(Edge->getName(), Edge);
        }
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

} // namespace ppar

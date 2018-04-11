#include "DDGPrinter.h"
#include "DotPrinter.h"

#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include <string>
#include <map>
#include <memory>
using namespace std;

char ppar::DDGPrinter::ID = 0;
RegisterPass<ppar::DDGPrinter> DDGPrinterRegister("dot-ddg", "Print DDG of a function to 'dot' file");

namespace ppar {

DDGPrinter::DDGPrinter() 
 : FunctionPass(ID) {}

bool DDGPrinter::runOnFunction(Function& F) {
    DataDependenceGraphPass& ddg = Pass::getAnalysis<DataDependenceGraphPass>();
    const Graph<Instruction*,ppar::Dependence*>& DG = ddg.getDDG();
    DotPrinter Printer("ddg");
    map<Instruction*,string> InstrToNodeName;
    
    for (Graph<Instruction*,ppar::Dependence*>::const_node_iterator node_it = DG.nodes_cbegin(); node_it != DG.nodes_cend(); node_it++) {
        GraphNode<Instruction*,ppar::Dependence*> DepNode = *node_it;
        Instruction* Instr = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[Instr] = Node->getName();

        Node->setAttribute( /* name = */ string("shape"), /* value = */ string("rectangle"));

        string str;
        raw_string_ostream rso(str);
        Instr->print(rso);
        Node->setAttribute( /* name = */ string("label"), /* value = */ str);

        Printer.addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = DG.edges_cbegin(); edge_it != DG.edges_cend(); edge_it++) {
        const std::pair<Instruction*,Instruction*>& NodePair = edge_it->first;
        const Graph<Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;

        for (const auto& DepEdge : EdgeSet) {
            Instruction* From = DepEdge.getFrom();
            Instruction* To = DepEdge.getTo();
            string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
            DotEdge* Edge = new DotEdge(EdgeName);

            Printer.addEdge(Edge->getName(), Edge);
        }
    }

    InstrToNodeName.clear();

    Printer.print();

    return false;
}

void DDGPrinter::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<DataDependenceGraphPass>();
}

};

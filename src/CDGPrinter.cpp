#include "CDGPrinter.h"
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

char ppar::CDGPrinter::ID = 0;
RegisterPass<ppar::CDGPrinter> CDGPrinterRegister("dot-cdg", "Print CDG of a function to 'dot' file");

namespace ppar {

CDGPrinter::CDGPrinter() 
 : FunctionPass(ID) {}

bool CDGPrinter::runOnFunction(Function& F) {
    ControlDependenceGraphPass& cdg = Pass::getAnalysis<ControlDependenceGraphPass>();
    const DependenceGraph<BasicBlock*,ppar::Dependence*>& DG = cdg.getCDG();
    DotPrinter Printer("cdg");
    map<BasicBlock*,string> BBToNodeName;
    
    for (DependenceGraph<BasicBlock*,ppar::Dependence*>::const_node_iterator node_it = DG.nodes_cbegin(); node_it != DG.nodes_cend(); node_it++) {
        DependenceGraphNode<BasicBlock*,ppar::Dependence*> DepNode = *node_it;
        BasicBlock* BB = DepNode.getNode();
        DotNode* Node = new DotNode();
        BBToNodeName[BB] = Node->getName();

        Node->setAttribute( /* name = */ string("shape"), /* value = */ string("rectangle"));

        string str;
        raw_string_ostream rso(str);
        BB->print(rso);
        Node->setAttribute( /* name = */ string("label"), /* value = */ str);

        Printer.addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = DG.edges_cbegin(); edge_it != DG.edges_cend(); edge_it++) {
        const std::pair<BasicBlock*,BasicBlock*>& NodePair = edge_it->first;
        const DependenceGraph<BasicBlock*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;

        for (const auto& DepEdge : EdgeSet) {
            BasicBlock* From = DepEdge.getFrom();
            BasicBlock* To = DepEdge.getTo();
            string EdgeName = BBToNodeName[From] + "->" + BBToNodeName[To];
            DotEdge* Edge = new DotEdge(EdgeName);

            Printer.addEdge(Edge->getName(), Edge);
        }
    }
    
    BBToNodeName.clear();

    Printer.print();

    return false;
}

void CDGPrinter::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<ControlDependenceGraphPass>();
}

};

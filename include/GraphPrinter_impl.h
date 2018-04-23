#ifndef PPAR_GRAPH_PRINTER_IMPL_H
#define PPAR_GRAPH_PRINTER_IMPL_H

#include "GraphPrinter.h"
#include "DotPrinter.h"

#include <string>

#define RegisterGraphPrinterPass(NODE,EDGE,PASS,VAR) \
\
RegisterPass<ppar::GraphPrinter<NODE,EDGE,PASS>> \
VAR( PASS::getGraphPassDotPrintOption(),\
     PASS::getGraphPassDotPrintOptionDesc() );

namespace ppar {

template <typename NODE, typename EDGE, typename PASS>
GraphPrinter<NODE,EDGE,PASS>::GraphPrinter() : FunctionPass(ID) {}
        
template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE,EDGE,PASS>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    llvm_unreachable("Class template GraphPrinter cannot be used directly (without concrete specialization)!");
    AU.setPreservesAll();
}

template <typename NODE, typename EDGE, typename PASS>
llvm::StringRef GraphPrinter<NODE,EDGE,PASS>::getPassName() const { 
    llvm_unreachable("Class template GraphPrinter cannot be used directly (without concrete specialization)!");
    return "Graph Printer"; 
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE,EDGE,PASS>::printDOTGraph(llvm::Function& F) {
    GraphPass<NODE,EDGE,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE,EDGE,PASS>>();
    const Graph<NODE,EDGE>& G = GPass.getGraph();
    DotPrinter Printer(F.getName().str() + PASS::getDOTFileExtension());
    map<NODE,std::string> InstrToNodeName;
    
    for (typename Graph<NODE,EDGE>::const_node_iterator node_it = G.nodes_cbegin(); node_it != G.nodes_cend(); node_it++) {
        GraphNode<NODE,EDGE> DepNode = *node_it;
        NODE N = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[N] = Node->getName();

        buildDotNode(N,Node);

        Printer.getGraph().addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = G.edges_cbegin(); edge_it != G.edges_cend(); edge_it++) {
        const std::pair<NODE,NODE>& NodePair = edge_it->first;
        const typename Graph<NODE,EDGE>::edge_set& EdgeSet = edge_it->second; 
        
        for (const auto DepEdge : EdgeSet) {
            NODE From = DepEdge.getFrom();
            NODE To = DepEdge.getTo();
            EDGE Dep = DepEdge.getData();
            std::string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
            DotEdge* Edge = new DotEdge(EdgeName);

            buildDotEdge(Dep,Edge);

            Printer.getGraph().addEdge(Edge->getName(), Edge);
        }
    }
    
    InstrToNodeName.clear();

    Printer.print();
}

template <typename NODE, typename EDGE, typename PASS>
bool GraphPrinter<NODE,EDGE,PASS>::runOnFunction(llvm::Function& F) {
    printDOTGraph(F);
    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PRINTER_IMPL_H

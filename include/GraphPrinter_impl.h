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
void GraphPrinter<NODE,EDGE,PASS>::printDOTGraph(llvm::Function& F, PrintType Type) {
    GraphPass<NODE,EDGE,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE,EDGE,PASS>>();
    const Graph<NODE,EDGE>& G = GPass.getGraph();
    map<NODE,std::string> InstrToNodeName;
    
    string FileName;
    if (Type == PrintType::PRINT_REGULAR) {
        FileName = F.getName().str() + PASS::getDotFileExtension();
    } else if (Type == PrintType::PRINT_DFS) {
        FileName = F.getName().str() + PASS::getDFSDotFileExtension();
    }
    DotPrinter Printer(FileName);
    
    if (Type == PrintType::PRINT_DFS) { 
        // augment graph with DFS ordering
        G.dfsTraverse();
    }
    auto& DFS_properties = G.getDFS_properties();

    for (typename Graph<NODE,EDGE>::const_node_iterator node_it = G.nodes_cbegin(); node_it != G.nodes_cend(); node_it++) {
        GraphNode<NODE,EDGE> DepNode = *node_it;
        NODE N = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[N] = Node->getName();

        buildDotNode(N,Node);
        
        if (Type == PrintType::PRINT_DFS) {
            // augment the node with DFS timestamp
            string DFS_timestamp = "[" + std::to_string(DFS_properties[DepNode]->TimestampEntry) + "/" 
                                       + std::to_string(DFS_properties[DepNode]->TimestampExit) + "]";
            Node->setAttribute(string("label"), DFS_timestamp + Node->getAttribute(string("label")));
        }

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
void GraphPrinter<NODE,EDGE,PASS>::printSCCsDOTGraph(llvm::Function& F) {
    GraphPass<NODE,EDGE,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE,EDGE,PASS>>();
    const Graph<NODE,EDGE>& G = GPass.getGraph();
    map<NODE,std::string> InstrToNodeName;
    string FileName;
    FileName = F.getName().str() + PASS::getSCCsDotFileExtension();
    DotPrinter Printer(FileName);

    auto& DFS_properties = G.getDFS_properties();

    for (typename Graph<NODE,EDGE>::const_sccs_iterator sccs_it = G.sccs_cbegin(); sccs_it != G.sccs_cend(); sccs_it++) {
        GraphNode<NODE,EDGE> SCCRoot = sccs_it->first;
        Graph<NODE,EDGE>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = new DotGraph(DotGraph::GraphType::SUBGRAPH, &Printer.getGraph());
        
        // print all nodes of the current SCC
        for (typename Graph<NODE,EDGE>::const_node_iterator node_it = SCC->nodes_cbegin(); node_it != SCC->nodes_cend(); node_it++) {
            GraphNode<NODE,EDGE> DepNode = *node_it;
            NODE N = DepNode.getNode();
            DotNode* Node = new DotNode();
            InstrToNodeName[N] = Node->getName();

            buildDotNode(N,Node);
        
            // augment the node with DFS timestamp
            string DFS_timestamp = "[" + std::to_string(DFS_properties[DepNode]->TimestampEntry) + "/" 
                                       + std::to_string(DFS_properties[DepNode]->TimestampExit) + "]";
            Node->setAttribute(string("label"), DFS_timestamp + Node->getAttribute(string("label")));
        
            SCCSubGraph->addNode(Node->getName(), Node);
        }
 
        for (typename Graph<NODE,EDGE>::const_node_iterator nodeA_it = SCC->nodes_cbegin(); nodeA_it != SCC->nodes_cend(); nodeA_it++) {
            GraphNode<NODE,EDGE> NodeA = *nodeA_it;
            for (typename Graph<NODE,EDGE>::const_node_iterator nodeB_it = SCC->nodes_cbegin(); nodeB_it != SCC->nodes_cend(); nodeB_it++) {
                GraphNode<NODE,EDGE> NodeB = *nodeB_it;
                const auto& G_edges = G.getEdges();
                const auto edges_it = G_edges.find(std::make_pair(NodeA.getNode(), NodeB.getNode()));
                if (edges_it != G_edges.end()) {
                    const typename Graph<NODE,EDGE>::edge_set& EdgeSet = edges_it->second;
                    for (const auto& DepEdge : EdgeSet) {
                        NODE From = DepEdge.getFrom();
                        NODE To = DepEdge.getTo();
                        EDGE Dep = DepEdge.getData();
                        string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
                        DotEdge* Edge = new DotEdge(EdgeName);
            
                        buildDotEdge(Dep,Edge);

                        SCCSubGraph->addEdge(Edge->getName(), Edge);
                    }
                }
            }
        }
       
        Printer.getGraph().addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    
    InstrToNodeName.clear();

    Printer.print();
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE,EDGE,PASS>::printComponentGraph(Function& F) {
    GraphPass<NODE,EDGE,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE,EDGE,PASS>>();
    const Graph<NODE,EDGE>& G = GPass.getGraph();
    map<NODE,std::string> InstrToNodeName;
    string FileName;
    FileName = F.getName().str() + PASS::getCGDotFileExtension();
    DotPrinter Printer(FileName);
    const Graph<NODE,EDGE>* CG = G.getComponentGraph();
    auto& DFS_properties = G.getDFS_properties();

    for (typename Graph<NODE,EDGE>::const_node_iterator node_it = CG->nodes_cbegin(); node_it != CG->nodes_cend(); node_it++) {
        GraphNode<NODE,EDGE> DepNode = *node_it;
        NODE N = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[N] = Node->getName();

        buildDotNode(N,Node);
        
        // augment the node with DFS timestamp
        string DFS_timestamp = "[" + std::to_string(DFS_properties[DepNode]->TimestampEntry) + "/" 
                                   + std::to_string(DFS_properties[DepNode]->TimestampExit) + "]";
        Node->setAttribute(string("label"), DFS_timestamp + Node->getAttribute(string("label")));

        Printer.getGraph().addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = CG->edges_cbegin(); edge_it != CG->edges_cend(); edge_it++) {
        const std::pair<NODE,NODE>& NodePair = edge_it->first;
        const typename Graph<NODE,EDGE>::edge_set& EdgeSet = edge_it->second; 
        
        for (const auto DepEdge : EdgeSet) {
            NODE From = DepEdge.getFrom();
            NODE To = DepEdge.getTo();
            EDGE Dep = DepEdge.getData();
            string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
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
    // print original graph + original augmented with DFS timestamps
    printDOTGraph(F);
    printDOTGraph(F, PrintType::PRINT_DFS);
    // compute SCCs and CG related information on the graph
    GraphPass<NODE,EDGE,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE,EDGE,PASS>>();
    const Graph<NODE,EDGE>& G = GPass.getGraph();
    G.findSCCs();
    G.buildComponentGraph();
    // print SCCs and CG of the original graph
    printComponentGraph(F);
    printSCCsDOTGraph(F);

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PRINTER_IMPL_H

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
GraphPrinter<NODE*,EDGE*,PASS>::GraphPrinter() : FunctionPass(ID) {}
        
template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE*,EDGE*,PASS>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    llvm_unreachable("Class template GraphPrinter cannot be used directly (without concrete specialization)!");
    AU.setPreservesAll();
}

template <typename NODE, typename EDGE, typename PASS>
llvm::StringRef GraphPrinter<NODE*,EDGE*,PASS>::getPassName() const { 
    llvm_unreachable("Class template GraphPrinter cannot be used directly (without concrete specialization)!");
    return "Graph Printer"; 
}

/*
 * Map in-memory ppar::Graph<NODE*,EDGE*> to DOT-format in-memory graph ppar::DotGraph
 */
template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE*,EDGE*,PASS>::formDOTGraph(DotGraph& DotG, const Graph<NODE*,EDGE*>& G, PrintType Type, PrintAugmentType AugmentType) {
    map<const NODE*,std::string> InstrToNodeName;
    
    if (AugmentType == PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS) { 
        // augment graph with DFS ordering
        if (!G.isDFSDataValid()) {
            G.dfsTraverse();
        }
    }
    auto& DFS_properties = G.getDFS_properties();

    for (typename Graph<NODE*,EDGE*>::const_node_iterator node_it = G.nodes_cbegin(); node_it != G.nodes_cend(); node_it++) {
        GraphNode<NODE*,EDGE*> DepNode = *node_it;
        const NODE* N = DepNode.getNode();
        DotNode* Node = new DotNode();
        InstrToNodeName[N] = Node->getName();

        buildDotNode(N,Node);
       
        if (Type == PrintType::PRINT_SCC_SUBGRAPH) {
            if (DepNode == G.getRoot()) {
                Node->setAttribute( /* name = */ string("fontsize"), /* value = */ string("20"));
                Node->setAttribute( /* name = */ string("penwidth"), /* value = */ string("3.0"));
            }
        }

        if (AugmentType == PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS) { 
            // augment the node with DFS timestamp
            string DFS_timestamp = "[" + std::to_string(DFS_properties[DepNode]->TimestampEntry) + "/" 
                                       + std::to_string(DFS_properties[DepNode]->TimestampExit) + "]";
            Node->setAttribute(string("label"), DFS_timestamp + Node->getAttribute(string("label")));
        }

        DotG.addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = G.edges_cbegin(); edge_it != G.edges_cend(); edge_it++) {
        const std::pair<const NODE*,const NODE*>& NodePair = edge_it->first;
        const typename Graph<NODE*,EDGE*>::edge_set& EdgeSet = edge_it->second; 
        
        for (const auto DepEdge : EdgeSet) {
            const NODE* From = DepEdge.getFrom();
            const NODE* To = DepEdge.getTo();
            EDGE* Dep = DepEdge.getData();
            std::string EdgeName = InstrToNodeName[From] + "->" + InstrToNodeName[To];
            DotEdge* Edge = new DotEdge(EdgeName);

            buildDotEdge(Dep,Edge);

            DotG.addEdge(Edge->getName(), Edge);
        }
    }
    
    InstrToNodeName.clear();
}

template <typename NODE, typename EDGE, typename PASS>
bool GraphPrinter<NODE*,EDGE*,PASS>::runOnFunction(llvm::Function& F) {

    if (F.isDeclaration()) return false;

    // get the main graph
    GraphPass<NODE*,EDGE*,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE*,EDGE*,PASS>>();
    const Graph<NODE*,EDGE*>& G = GPass.getGraph();
    string FileName;
    
    // print the main graph of the function
    FileName = F.getName().str() + PASS::getDotFileExtension();
    DotPrinter MainPrinter(FileName);
    formDOTGraph(MainPrinter.getGraph(), G, PrintType::PRINT_MAIN_GRAPH); 
    MainPrinter.print();

    // augment the main graph of the function with 
    // DFS timestamps and print it
    FileName = F.getName().str() + PASS::getDFSDotFileExtension();
    DotPrinter MainPrinter_dfs(FileName);
    formDOTGraph(MainPrinter_dfs.getGraph(), G, PrintType::PRINT_MAIN_GRAPH, PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS); 
    MainPrinter_dfs.print();

    if (!G.isSCCsDataValid() || !G.isComponentGraphDataValid()) {
        G.findSCCs();
        G.buildComponentGraph();
    }

    // print component graph of the function
    FileName = F.getName().str() + PASS::getCGDotFileExtension();
    DotPrinter ComponentGraphPrinter(FileName);
    formDOTGraph(ComponentGraphPrinter.getGraph(), *G.getComponentGraph(), PrintType::PRINT_MAIN_GRAPH, PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS); 
    ComponentGraphPrinter.print();

    // print all SCCs of the graph
    FileName = F.getName().str() + PASS::getSCCsDotFileExtension();
    DotPrinter SCCsPrinter(FileName);
    for (typename Graph<NODE*,EDGE*>::const_sccs_iterator sccs_it = G.sccs_cbegin(); sccs_it != G.sccs_cend(); sccs_it++) {
        GraphNode<NODE*,EDGE*> SCCRoot = sccs_it->first;
        Graph<NODE*,EDGE*>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = new DotGraph(DotGraph::GraphType::SUBGRAPH, &SCCsPrinter.getGraph());
    
        formDOTGraph(*SCCSubGraph, *SCC, PrintType::PRINT_SCC_SUBGRAPH, PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS);
        SCCsPrinter.getGraph().addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    SCCsPrinter.print();

    // print all SCCs of the graph
    FileName = F.getName().str() + PASS::getSCCsCGDotFileExtension();
    DotPrinter SCCsCGPrinter(FileName);
    for (typename Graph<NODE*,EDGE*>::const_sccs_iterator sccs_it = G.sccs_cbegin(); sccs_it != G.sccs_cend(); sccs_it++) {
        GraphNode<NODE*,EDGE*> SCCRoot = sccs_it->first;
        Graph<NODE*,EDGE*>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = new DotGraph(DotGraph::GraphType::SUBGRAPH, &SCCsPrinter.getGraph());
    
        formDOTGraph(*SCCSubGraph, *SCC, PrintType::PRINT_SCC_SUBGRAPH, PrintAugmentType::PRINT_AUGMENT_DFS_TIMESTAMPS);
        SCCsPrinter.getGraph().addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    SCCsCGPrinter.print();

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PRINTER_IMPL_H

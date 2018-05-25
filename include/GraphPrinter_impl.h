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
void GraphPrinter<NODE*,EDGE*,PASS>::formDOTGraph(DotGraph& DotG, const Graph<NODE*,EDGE*>& G, uint64_t FormOptions) {
    std::unordered_map<const void*,std::string>& NodeToDotName = DotG.getMapping();

    auto& DFS_properties = (G.getParent() != nullptr) ? (G.getParent())->getDFS_properties() : G.getDFS_properties();

    if ((FormOptions & FormationOption::ONLY_EDGES) == 0x0) { 
        for (typename Graph<NODE*,EDGE*>::const_node_iterator node_it = G.nodes_cbegin(); node_it != G.nodes_cend(); node_it++) {
            GraphNode<NODE*,EDGE*> DepNode = *node_it;
            const NODE* N = DepNode.getNode();
            DotNode* Node = new DotNode();
            NodeToDotName[static_cast<const void*>(N)] = Node->getName();

            buildDotNode(N,Node);
       
            if (FormOptions & FormationOption::MARK_SCC_ROOTS) { 
                if (DepNode == G.getRoot()) {
                    Node->setAttribute( /* name = */ string("fontsize"), /* value = */ string("20"));
                    Node->setAttribute( /* name = */ string("penwidth"), /* value = */ string("3.0"));
                }
            }

            if (FormOptions & FormationOption::DFS_TIMESTAMPS) {
                // augment the node with DFS timestamp
                string DFS_timestamp = "[" + std::to_string(DFS_properties[DepNode]->TimestampEntry) + "/" 
                                          + std::to_string(DFS_properties[DepNode]->TimestampExit) + "]";
                Node->setAttribute(string("label"), DFS_timestamp + Node->getAttribute(string("label")));
            }
            
            DotG.addNode(Node->getName(), Node);
        }
    }
    
    if ((FormOptions & FormationOption::ONLY_NODES) == 0x0) { 
        for (auto edge_it = G.edges_cbegin(); edge_it != G.edges_cend(); edge_it++) {
            const std::pair<const NODE*,const NODE*>& NodePair = edge_it->first;
            const typename Graph<NODE*,EDGE*>::edge_set& EdgeSet = edge_it->second; 
        
            for (const auto DepEdge : EdgeSet) {
                const NODE* From = DepEdge.getFrom();
                const NODE* To = DepEdge.getTo();
                EDGE* Dep = DepEdge.getData();
                std::string EdgeName = NodeToDotName[static_cast<const void*>(From)] + "->" + NodeToDotName[static_cast<const void*>(To)];
                DotEdge* Edge = new DotEdge(EdgeName);

                buildDotEdge(Dep,Edge);

                DotG.addEdge(Edge->getName(), Edge);
            }
        }
    } 
}

template <typename NODE, typename EDGE, typename PASS>
void GraphPrinter<NODE*,EDGE*,PASS>::printDotGraphs(const Graph<NODE*,EDGE*>& G, std::string Name) {
    string FileName;
    uint64_t PrintConfig;

    if (!G.isDFSDataValid()) {
        G.dfsTraverse();
    }

    if (!G.isSCCsDataValid() || !G.isComponentGraphDataValid()) {
        G.findSCCs();
        G.buildComponentGraph();
    }

    // print the main graph of the function
    FileName = Name + PASS::getDotFileExtension();
    DotPrinter MainPrinter(FileName);
    PrintConfig = FormationOption::DEFAULT;
    formDOTGraph(MainPrinter.getGraph(), G, PrintConfig); 
    MainPrinter.print();

    // augment the main graph of the function with 
    // DFS timestamps and print it
    FileName = Name + PASS::getDFSDotFileExtension();
    DotPrinter MainPrinter_dfs(FileName);
    PrintConfig = FormationOption::DEFAULT + FormationOption::DFS_TIMESTAMPS;
    formDOTGraph(MainPrinter_dfs.getGraph(), G, PrintConfig); 
    MainPrinter_dfs.print();

    // print component graph of the function
    FileName = Name + PASS::getCGDotFileExtension();
    DotPrinter ComponentGraphPrinter(FileName);
    PrintConfig = FormationOption::DEFAULT + 
                  FormationOption::DFS_TIMESTAMPS +
                  FormationOption::MARK_SCC_ROOTS;
    formDOTGraph(ComponentGraphPrinter.getGraph(), *G.getComponentGraph(), PrintConfig); 
    ComponentGraphPrinter.print();

    // print all SCCs of the graph
    FileName = Name + PASS::getSCCsDotFileExtension();
    DotPrinter SCCsPrinter(FileName);
    PrintConfig = FormationOption::DEFAULT + 
                  FormationOption::DFS_TIMESTAMPS +
                  FormationOption::MARK_SCC_ROOTS;
    for (typename Graph<NODE*,EDGE*>::const_sccs_iterator sccs_it = G.sccs_cbegin(); sccs_it != G.sccs_cend(); sccs_it++) {
        GraphNode<NODE*,EDGE*> SCCRoot = sccs_it->first;
        Graph<NODE*,EDGE*>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = (SCCsPrinter.getGraph()).createSubGraph();
        formDOTGraph(*SCCSubGraph, *SCC, PrintConfig);
        SCCsPrinter.getGraph().addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    SCCsPrinter.print();

    // print all SCCs of the graph
    FileName = Name + PASS::getSCCsCGDotFileExtension();
    DotPrinter SCCsCGPrinter(FileName);
    PrintConfig = FormationOption::DEFAULT +
                  FormationOption::DFS_TIMESTAMPS +
                  FormationOption::MARK_SCC_ROOTS +
                  FormationOption::ONLY_NODES;
    for (typename Graph<NODE*,EDGE*>::const_sccs_iterator sccs_it = G.sccs_cbegin(); sccs_it != G.sccs_cend(); sccs_it++) {
        GraphNode<NODE*,EDGE*> SCCRoot = sccs_it->first;
        Graph<NODE*,EDGE*>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = (SCCsCGPrinter.getGraph()).createSubGraph();
        formDOTGraph(*SCCSubGraph, *SCC, PrintConfig);
        SCCsCGPrinter.getGraph().addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    PrintConfig = FormationOption::DEFAULT + 
                  FormationOption::ONLY_EDGES;
    formDOTGraph(SCCsCGPrinter.getGraph(), G, PrintConfig);
    SCCsCGPrinter.print();
}

template <typename NODE, typename EDGE, typename PASS>
bool GraphPrinter<NODE*,EDGE*,PASS>::runOnFunction(llvm::Function& F) {

    if (F.isDeclaration()) return false;

    // get the main graph
    GraphPass<NODE*,EDGE*,PASS>& GPass = llvm::Pass::getAnalysis<GraphPass<NODE*,EDGE*,PASS>>();
    const Graph<NODE*,EDGE*>& G = GPass.getFunctionGraph();
    printDotGraphs(G, F.getName().str());
   
    int i = 0;
    for (typename GraphPass<NODE*,EDGE*,PASS>::const_loop_graph_iterator loop_graph_it = GPass.cbegin(); 
         loop_graph_it != GPass.cend(); ++loop_graph_it) {
        const Graph<NODE*,EDGE*>& LG = *(loop_graph_it->second);
        printDotGraphs(LG, F.getName().str() + "loop" + std::to_string(i));
        i++;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_GRAPH_PRINTER_IMPL_H

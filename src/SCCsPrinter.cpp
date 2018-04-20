#include "SCCsPrinter.h"
#include "DotPrinter.h"

#include <string>
#include <algorithm>
using namespace std;

#include "llvm/PassRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

char ppar::SCCsPrinter::ID = 0;
RegisterPass<ppar::SCCsPrinter> SCCsPrinterRegister("dot-sccs", "Print SCCs of PDG of the function to the 'dot' file");

namespace ppar {

SCCsPrinter::SCCsPrinter() 
    : FunctionPass(ID) {}

void SCCsPrinter::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<SCCsGraphPass>();
}

void SCCsPrinter::printComponentGraph(Function& F) {
    // get Program Dependence Graph (PDG) with SCCs-related information
    SCCsGraphPass& sccs = Pass::getAnalysis<SCCsGraphPass>();
    const Graph<Instruction*,ppar::Dependence*>& DG = sccs.getPDG();
    // print Component Graph (CG)
    const Graph<Instruction*,ppar::Dependence*>* CG = DG.getComponentGraph();
    DotPrinter Printer(F.getName().str() + ".pdg.cg");
    DotGraph& DotGraph = Printer.getGraph();
    map<Instruction*,string> InstrToNodeName;

    for (Graph<Instruction*,ppar::Dependence*>::const_node_iterator node_it = CG->nodes_cbegin(); node_it != CG->nodes_cend(); node_it++) {
        GraphNode<Instruction*,ppar::Dependence*> DepNode = *node_it;
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

        Printer.getGraph().addNode(Node->getName(), Node);
    }

    // print all graph edges
    for (auto edge_it = CG->edges_cbegin(); edge_it != CG->edges_cend(); edge_it++) {
        const std::pair<Instruction*,Instruction*>& NodePair = edge_it->first;
        const Graph<Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second; 
        
        for (const auto DepEdge : EdgeSet) {
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

            Printer.getGraph().addEdge(Edge->getName(), Edge);
        }
    }
    
    InstrToNodeName.clear();

    Printer.print();
}

void SCCsPrinter::printSCCs(Function& F) {
    // get Program Dependence Graph (PDG) with SCCs-related information
    SCCsGraphPass& sccs = Pass::getAnalysis<SCCsGraphPass>();
    const Graph<Instruction*,ppar::Dependence*>& DG = sccs.getPDG();
    // create SCCs printer
    DotPrinter Printer(F.getName().str() + ".pdg.sccs");
    DotGraph& PrinterGraph = Printer.getGraph();
    map<Instruction*,string> InstrToNodeName;

    for (Graph<Instruction*,ppar::Dependence*>::const_sccs_iterator sccs_it = DG.sccs_cbegin(); sccs_it != DG.sccs_cend(); sccs_it++) {
        GraphNode<Instruction*,ppar::Dependence*> SCCRoot = sccs_it->first;
        Graph<Instruction*,ppar::Dependence*>* SCC = sccs_it->second;
        DotGraph* SCCSubGraph = new DotGraph(DotGraph::GraphType::SUBGRAPH, &Printer.getGraph());
                    
        for (Graph<Instruction*,ppar::Dependence*>::const_node_iterator node_it = SCC->nodes_cbegin(); node_it != SCC->nodes_cend(); node_it++) {
            GraphNode<Instruction*,ppar::Dependence*> DepNode = *node_it;
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

            SCCSubGraph->addNode(Node->getName(), Node);
        }
        
        // print all graph edges
        for (auto edge_it = DG.edges_cbegin(); edge_it != DG.edges_cend(); edge_it++) {
            const std::pair<Instruction*,Instruction*>& NodePair = edge_it->first;
            const Graph<Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second; 
        
            for (const auto DepEdge : EdgeSet) {
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

                SCCSubGraph->addEdge(Edge->getName(), Edge);
            }
        }
        
        PrinterGraph.addSubGraph(SCCSubGraph->getName(), SCCSubGraph);
    }
    
    InstrToNodeName.clear();

    Printer.print();
}

bool SCCsPrinter::runOnFunction(Function& F) {
    // print Component Graph (CG)
    printComponentGraph(F);
    // print Strongly Connected Components (SCCs)
    printSCCs(F);

    return false;
}

} // namespace ppar

#include "DotPrinter.h"

#include <algorithm>

using namespace std;

namespace ppar {

DotNode::DotNode() {
    Name = "node" + to_string(NodeNum);
    NodeNum++;
    Attributes.clear();
}

DotNode::~DotNode() {
    Attributes.clear();
}

void DotNode::setAttribute(string Name, string Value) {
    Attributes[Name] = Value;
}
        
string DotNode::getAttribute(string Name) {
    unordered_map<string,string>::const_iterator it = Attributes.find(Name);
    if (it != Attributes.cend()) {
        return Attributes[Name];
    } else {
        return string("");
    }
}

void DotNode::print(std::ofstream& DotFile) const {
    DotFile << Name << " [";
    uint64_t i = 0; 
    for (unordered_map<string,string>::const_iterator attr_it = Attributes.cbegin(); attr_it != Attributes.cend(); attr_it++) {
        if (i != 0) {
            // print attribute delimeter
            DotFile << ",";
        }
        DotFile << attr_it->first << "=";
        if (attr_it->first == "label") {
            DotFile << "\"" << attr_it->second << "\"";
        } else {
            DotFile << attr_it->second;
        }
        i++;
    }
    DotFile << " ]\n";
}

uint64_t DotNode::NodeNum = 0;

DotEdge::DotEdge(string N) {
    Name = N;
    Attributes.clear();
}

DotEdge::~DotEdge() {
    Attributes.clear();
}

void DotEdge::setAttribute(string Name, string Value) {
    Attributes[Name] = Value;
}
        
string DotEdge::getAttribute(string Name) {
    unordered_map<string,string>::const_iterator it = Attributes.find(Name);
    if (it != Attributes.cend()) {
        return Attributes[Name];
    } else {
        return string("");
    }
}

void DotEdge::print(std::ofstream& DotFile) const {
    DotFile << Name << " [";
    uint64_t i = 0; 
    for (unordered_map<string,string>::const_iterator attr_it = Attributes.cbegin(); attr_it != Attributes.cend(); attr_it++) {
        if (i != 0) {
            // print attribute delimeter
            DotFile << ",";
        }
        DotFile << attr_it->first << "=";
        if (attr_it->first == "label") {
            DotFile << "\"" << attr_it->second << "\"";
        } else {
            DotFile << attr_it->second;
        }
        i++;
    }
    DotFile << " ]\n";
}

DotGraph::DotGraph(GraphType Type, DotGraph* ParentGraph)
 : Type(Type), Graph(ParentGraph) {
    
    SubGraphNum = 0;
    
    if (Type == GraphType::DIRECTED_GRAPH) {
        Name = "G";
    } else {
        Name = "cluster" + to_string(ParentGraph->SubGraphNum);
    }

    Attributes.clear();
    Nodes.clear();
    Edges.clear();
    SubGraphs.clear();
    NodeToDotName.clear(); 
}

DotGraph::~DotGraph() {
    
    for (unordered_map<string,DotNode*>::const_iterator node_it = Nodes.cbegin(); node_it != Nodes.cend(); node_it++) {
        if (node_it->second != nullptr) {
            delete node_it->second;   
        }
    }
            
    for (unordered_map<string,DotEdge*>::const_iterator edge_it = Edges.cbegin(); edge_it != Edges.cend(); edge_it++) {
        if (edge_it->second != nullptr) {
            delete edge_it->second;   
        }
    }

    for (unordered_map<string,DotGraph*>::const_iterator subgraph_it = SubGraphs.cbegin(); subgraph_it != SubGraphs.cend(); subgraph_it++) {
        if (subgraph_it->second != nullptr) {
            delete subgraph_it->second;   
        }
    }

    Nodes.clear();
    Edges.clear();
    SubGraphs.clear();
    NodeToDotName.clear(); 
}

DotGraph* DotGraph::createSubGraph() {
    DotGraph* SubGraph = new DotGraph(DotGraph::GraphType::SUBGRAPH, this);
    SubGraphNum++;
    return SubGraph;
}

void DotGraph::addNode(string Name, DotNode* Node) {
    Nodes[Name] = Node;
}

void DotGraph::addEdge(string Name, DotEdge* Edge) {
    Edges[Name] = Edge;
}

void DotGraph::addSubGraph(string Name, DotGraph* SubGraph) {
    SubGraphs[Name] = SubGraph;
    
    // propagate mapping information from child to a parent
    for (auto it = (SubGraph->getMapping()).begin(); it != (SubGraph->getMapping()).end(); ++it) {
        NodeToDotName[it->first] = it->second;
    }
}

void DotGraph::print(std::ofstream& DotFile) const {

    DotFile << "\n"; 
    // print graph's name
    if (this->Type == GraphType::DIRECTED_GRAPH) {
        DotFile << "digraph " << Name << " {\n"; 
    } else if (this->Type == GraphType::SUBGRAPH) {
        DotFile << "subgraph " << Name << " {\n"; 
    }
    DotFile << "\n"; 

    for (unordered_map<string,string>::const_iterator attr_it = Attributes.cbegin(); attr_it != Attributes.cend(); attr_it++) {
        DotFile << attr_it->first << "=";
        if (attr_it->first == "label") {
            DotFile << "\"" << attr_it->second << "\"";
        } else {
            DotFile << attr_it->second;
        }
        DotFile << ";\n";
    }
    DotFile << "\n"; 

    // print the top-level nodes
    for (unordered_map<string,DotNode*>::const_iterator node_it = Nodes.cbegin(); node_it != Nodes.cend(); node_it++) {
        node_it->second->print(DotFile);
    }
    DotFile << "\n"; 
            
    // print the top-level edges
    for (unordered_map<string,DotEdge*>::const_iterator edge_it = Edges.cbegin(); edge_it != Edges.cend(); edge_it++) {
        edge_it->second->print(DotFile);
    }
    DotFile << "\n"; 

    // print constituent subgraphs
    for (unordered_map<string,DotGraph*>::const_iterator subgraph_it = SubGraphs.cbegin(); subgraph_it != SubGraphs.cend(); subgraph_it++) {
        subgraph_it->second->print(DotFile);
    }
    DotFile << "\n"; 
    
    DotFile << "}";
    DotFile << "\n"; 
}

DotPrinter::DotPrinter(string Name) 
 : Graph(DotGraph::GraphType::DIRECTED_GRAPH), FileName(Name) {}

DotPrinter::~DotPrinter() {}

void DotPrinter::print() const {
    std::ofstream DotFile(FileName + ".dot");
    Graph.print(DotFile);
    DotFile.close();
}

} // namespace ppar

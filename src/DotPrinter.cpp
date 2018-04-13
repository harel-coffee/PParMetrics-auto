#include "DotPrinter.h"

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

string DotNode::getName() const { return Name; }

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

string DotEdge::getName() const { return Name; }

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

DotSubGraph::DotSubGraph(DotDiGraph& Parent) 
 : ParentGraph(Parent) {
    Name = "cluster" + to_string(SubGraphNum);
    SubGraphNum++;
    Attributes.clear();
    Nodes.clear();
    Edges.clear();
}

DotSubGraph::~DotSubGraph() {
    
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

    Nodes.clear();
    Edges.clear();
}

void DotSubGraph::addNode(string Name, DotNode* Node) {
    Nodes[Name] = Node;
}

void DotSubGraph::addEdge(string Name, DotEdge* Edge) {
    Edges[Name] = Edge;
}

void DotSubGraph::print(std::ofstream& DotFile) const {
    DotFile << "subgraph " << Name << " {\n"; 
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
    DotFile << "}";
}

uint64_t DotSubGraph::SubGraphNum = 0;

DotDiGraph::DotDiGraph(DotPrinter& Printer, string GraphName) 
 : Name(GraphName), Printer(Printer) {
    Attributes.clear();
    Nodes.clear();
    Edges.clear();
    SubGraphs.clear();
}

DotDiGraph::~DotDiGraph() {
    
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

    for (unordered_map<string,DotSubGraph*>::const_iterator subgraph_it = SubGraphs.cbegin(); subgraph_it != SubGraphs.cend(); subgraph_it++) {
        if (subgraph_it->second != nullptr) {
            delete subgraph_it->second;   
        }
    }

    Nodes.clear();
    Edges.clear();
    SubGraphs.clear();
}

void DotDiGraph::addNode(string Name, DotNode* Node) {
    Nodes[Name] = Node;
}

void DotDiGraph::addEdge(string Name, DotEdge* Edge) {
    Edges[Name] = Edge;
}

void DotDiGraph::addSubGraph(string Name, DotSubGraph* SubGraph) {
    SubGraphs[Name] = SubGraph;
}

void DotDiGraph::print(std::ofstream& DotFile) const {
    DotFile << "digraph " << GraphName << " {\n"; 
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
    for (unordered_map<string,DotSubGraph*>::const_iterator subgraph_it = SubGraphs.cbegin(); subgraph_it != SubGraphs.cend(); subgraph_it++) {
        subgraph_it->second->print(DotFile);
    }

    DotFile << "\n"; 
    DotFile << "}";
}

DotPrinter::DotPrinter(string Name) 
 : Graph(string("G")), FileName(Name) {}

DotPrinter::~DotPrinter() {}

void DotPrinter::print() const {
    std::ofstream DotFile(FileName + ".dot");
    Graph.print(DotFile);
    DotFile.close();
}

} // namespace ppar

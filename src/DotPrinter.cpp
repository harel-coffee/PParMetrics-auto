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

DotPrinter::DotPrinter(string Name) : FileName(Name) {
    Nodes.clear();
    Edges.clear();
}

DotPrinter::~DotPrinter() {

    for (unordered_map<string,DotNode*>::iterator node_it = Nodes.begin(); node_it != Nodes.end(); node_it++) {
        delete node_it->second;
    }

    for (unordered_map<string,DotEdge*>::iterator edge_it = Edges.begin(); edge_it != Edges.end(); edge_it++) {
        delete edge_it->second;
    }

    Nodes.clear();
    Edges.clear();
}

void DotPrinter::addNode(string Name, DotNode* Node) {
    Nodes[Name] = Node;
}

void DotPrinter::addEdge(string Name, DotEdge* Edge) {
    Edges[Name] = Edge;
}

void DotPrinter::print() const {
    std::ofstream DotFile(FileName + ".dot");
    DotFile << "digraph {\n"; 
    DotFile << "\n"; 
            
    for (unordered_map<string,DotNode*>::const_iterator node_it = Nodes.cbegin(); node_it != Nodes.cend(); node_it++) {
        node_it->second->print(DotFile);
    }
            
    DotFile << "\n"; 
            
    for (unordered_map<string,DotEdge*>::const_iterator edge_it = Edges.cbegin(); edge_it != Edges.cend(); edge_it++) {
        edge_it->second->print(DotFile);
    }

    DotFile << "\n"; 
    DotFile << "}";
    DotFile.close();
}

} // namespace ppar

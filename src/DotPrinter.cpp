#include "DotPrinter.h"

#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

namespace ppar {

DotNode::DotNode() {
    name = "node" + to_string(node_num);
    node_num++;
    attributes.clear();
}

DotNode::~DotNode() {
    attributes.clear();
}

void DotNode::setAttribute(string name, string value) {
    attributes[name] = value;
}
        
string DotNode::getAttribute(string name) {
    unordered_map<string,string>::const_iterator it = attributes.find(name);
    if (it != attributes.cend()) {
        return attributes[name];
    } else {
        return string("");
    }
}

string DotNode::getName() const { return name; }

void DotNode::print() const {
    outs() << name << " [";
    uint64_t i = 0; 
    for (unordered_map<string,string>::const_iterator attr_it = attributes.cbegin(); attr_it != attributes.cend(); attr_it++) {
        if (i != 0 && i != attributes.size()-1) {
            // print attribute delimeter
            outs() << ",";
        }
        outs() << attr_it->first << "=" << attr_it->second;
        i++;
    }
    outs() << " ]\n";
}

uint64_t DotNode::node_num = 0;

DotEdge::DotEdge(string n) {
    name = n;
    attributes.clear();
}

DotEdge::~DotEdge() {
    attributes.clear();
}

void DotEdge::setAttribute(string name, string value) {
    attributes[name] = value;
}
        
string DotEdge::getAttribute(string name) {
    unordered_map<string,string>::const_iterator it = attributes.find(name);
    if (it != attributes.cend()) {
        return attributes[name];
    } else {
        return string("");
    }
}

string DotEdge::getName() const { return name; }

void DotEdge::print() const {
    outs() << name << " [";
    uint64_t i = 0; 
    for (unordered_map<string,string>::const_iterator attr_it = attributes.cbegin(); attr_it != attributes.cend(); attr_it++) {
        if (i != 0 && i != attributes.size()-1) {
            // print attribute delimeter
            outs() << ",";
        }
        outs() << attr_it->first << "=" << attr_it->second;
        i++;
    }
    outs() << " ]\n";
}

DotPrinter::DotPrinter() {
    nodes.clear();
    edges.clear();
}

DotPrinter::~DotPrinter() {

    for (unordered_map<string,DotNode*>::iterator node_it = nodes.begin(); node_it != nodes.end(); node_it++) {
        delete node_it->second;
    }

    for (unordered_map<string,DotEdge*>::iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++) {
        delete edge_it->second;
    }

    nodes.clear();
    edges.clear();
}

void DotPrinter::addNode(string name, DotNode* dotNode) {
    nodes[name] = dotNode;
}

void DotPrinter::addEdge(string name, DotEdge* dotEdge) {
    edges[name] = dotEdge;
}

void DotPrinter::print() const {
    outs() << "digraph {\n"; 
    outs() << "\n"; 
            
    for (unordered_map<string,DotNode*>::const_iterator node_it = nodes.cbegin(); node_it != nodes.cend(); node_it++) {
        node_it->second->print();
    }
            
    outs() << "\n"; 
            
    for (unordered_map<string,DotEdge*>::const_iterator edge_it = edges.cbegin(); edge_it != edges.cend(); edge_it++) {
        edge_it->second->print();
    }

    outs() << "\n"; 
    outs() << "}"; 
}

} // namespace ppar

#ifndef PPAR_DOT_PRINTER_H
#define PPAR_DOT_PRINTER_H

#include "ppar_common_includes.h"

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

namespace ppar {

class DotNode {
    
    using string = std::string;

    public:
        DotNode(); 
        ~DotNode(); 
        
        string getAttribute(string Name);
        void setAttribute(string Name, string Value);
        
        string getName() const { return Name; }
        
        void print(std::ofstream& File) const;

    private:
        // node name of the format 'node%d'
        static uint64_t NodeNum;
        string Name;
        // node attributes 
        std::unordered_map<string,string> Attributes;
};

class DotEdge {
    
    using string = std::string;

    public:
        DotEdge(string Name); 
        ~DotEdge(); 
        
        string getAttribute(string Name);
        void setAttribute(string Name, string Value);
        
        string getName() const { return Name; }
        
        void print(std::ofstream& File) const;

    private:
        // edge name of the "from->to" format
        string Name;
        // edge attributes        
        std::unordered_map<string,string> Attributes;
};

class DotSubGraph {

    using string = std::string;

    public:
        DotSubGraph(DotGraph& ParentGraph);
        ~DotSubGraph();

        void addNode(string Name, DotNode* DotNode);
        void addEdge(string Name, DotEdge* DotEdge);

        string getAttribute(string Name);
        void setAttribute(string Name, string Value);

        DotNode* getNode(string Name) { return Nodes[Name]; }
        DotEdge* getEdge(string Name) { return Edges[Name]; }

        string getName() const { return Name; }
        DotDiGraph& getParentGraph() const { return Graph; }
        
        void print() const;

    private:
        // subgraph name of the format 'cluster%d'
        static uint64_t SubGraphNum;
        string Name;
        // reference to the parent
        DotGraph& ParentGraph;
        // composing nodes and edges of the subgraph
        std::unordered_map<string,DotNode*> Nodes;
        std::unordered_map<string,DotEdge*> Edges;
        // subgraph attributes
        std::unordered_map<string,string> Attributes;
};

class DotDiGraph {

    using string = std::string;

    public:
        DotDiGraph(DotPrinter& Printer, string GraphName);
        ~DotDiGraph();

        void addNode(string Name, DotNode* DotNode);
        void addEdge(string Name, DotEdge* DotEdge);
        void addSubGraph(string Name, DotSubGraph* SubGraph);

        string getAttribute(string Name);
        void setAttribute(string Name, string Value);

        DotNode* getNode(string Name) { return Nodes[Name]; }
        DotEdge* getEdge(string Name) { return Edges[Name]; }
        DotSubGraph* getSubGraph(string Name) { return SubGraph[Name]; }

        string getName() const;

        void print() const;

    private:
        string Name;
        DotPrinter& Printer;
        // top-level nodes and edges
        std::unordered_map<string,DotNode*> Nodes;
        std::unordered_map<string,DotEdge*> Edges;
        // constituent subgraphs
        std::unordered_map<string,DotSubGraph*> SubGraphs;
        // attributes of the graph
        std::unordered_map<string,string> Attributes;
};

class DotPrinter {

    using string = std::string;

    public:
        DotPrinter(string FileName);
        ~DotPrinter();

        DotDiGraph& getGraph { return Graph; }

        void print() const;

    private:
        // name of the file to print 
        string FileName;
        // top-level digraph
        DotDiGraph Graph;
};

} // namespace ppar

#endif //#ifndef PPAR_DOT_PRINTER_H

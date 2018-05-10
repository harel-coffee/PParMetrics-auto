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

class DotGraph {

    using string = std::string;

    public:
        
        enum class GraphType {
            DIRECTED_GRAPH,
            SUBGRAPH,
            UNKNOWN
        };
    
        DotGraph(GraphType Type = GraphType::DIRECTED_GRAPH, DotGraph* ParentGraph = nullptr);
        ~DotGraph();

        DotGraph* createSubGraph();

        void addNode(string Name, DotNode* DotNode);
        void addEdge(string Name, DotEdge* DotEdge);
        void addSubGraph(string Name, DotGraph* DotGraph);

        string getAttribute(string Name);
        void setAttribute(string Name, string Value);

        DotNode* getNode(string Name) { return Nodes[Name]; }
        DotEdge* getEdge(string Name) { return Edges[Name]; }
        DotGraph* getSubGraph(string Name) { return SubGraphs[Name]; }

        string getName() const { return Name; }
        DotGraph* getParentGraph() const { return Graph; }
        
        void print(std::ofstream& DotFile) const;

        void establishMapping(const void* Node, std::string Name) {
            NodeToDotName[Node] = Name;
        }

        std::unordered_map<const void*,std::string>& getMapping() {
            return NodeToDotName;
        }

    private:
        // subgraph name of the format 'cluster%d'
        uint64_t SubGraphNum;
        string Name;
        // type of the graph
        GraphType Type;
        // pointer to the parent
        DotGraph* Graph;
        // composing nodes and edges of the subgraph
        std::unordered_map<string,DotNode*> Nodes;
        std::unordered_map<string,DotEdge*> Edges;
        // constituent subgraphs
        std::unordered_map<string,DotGraph*> SubGraphs;
        // subgraph attributes
        std::unordered_map<string,string> Attributes;
        // used during the DOT representation construction
        std::unordered_map<const void*,std::string> NodeToDotName;
};

class DotPrinter {

    using string = std::string;

    public:
        DotPrinter(string FileName);
        ~DotPrinter();

        DotGraph& getGraph() { return Graph; }

        void print() const;

    private:
        // name of the file to print 
        string FileName;
        // top-level digraph
        DotGraph Graph;
};

} // namespace ppar

#endif //#ifndef PPAR_DOT_PRINTER_H

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
        void setAttribute(string Name, string Value);
        string getAttribute(string Name);
        string getName() const;
        void print(std::ofstream& File) const;

    private:
        static uint64_t NodeNum;
        std::unordered_map<string,string> Attributes;
        string Name;
};

class DotEdge {
    
    using string = std::string;

    public:
        DotEdge(string Name); 
        ~DotEdge(); 
        void setAttribute(string Name, string Value);
        string getAttribute(string Name);
        string getName() const;
        void print(std::ofstream& File) const;

    private:
        std::unordered_map<string,string> Attributes;
        string Name; // in "from->to" format
};

class DotPrinter {

    using string = std::string;

    public:
        DotPrinter(string FileName);
        ~DotPrinter();

        void addNode(string Name, DotNode* DotNode);
        void addEdge(string Name, DotEdge* DotEdge);
        void print() const;

    private:
        std::unordered_map<string,DotNode*> Nodes;
        std::unordered_map<string,DotEdge*> Edges;
        string FileName;
};

} // namespace ppar

#endif // #ifndef DOT_PRINTER_H

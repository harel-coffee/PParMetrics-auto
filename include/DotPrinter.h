#ifndef PPAR_DOT_PRINTER_H
#define PPAR_DOT_PRINTER_H

#include <unordered_map>
#include <string>

#include <cstdint>

namespace ppar {

class DotNode {
    
    using string = std::string;

    public:
        DotNode(); 
        ~DotNode(); 
        void setAttribute(string name, string value);
        string getAttribute(string name);
        string getName() const;
        void print() const;

    private:
        static uint64_t node_num;
        std::unordered_map<string,string> attributes;
        string name;
};

class DotEdge {
    
    using string = std::string;

    public:
        DotEdge(string name); 
        ~DotEdge(); 
        void setAttribute(string name, string value);
        string getAttribute(string name);
        string getName() const;
        void print() const;

    private:
        std::unordered_map<string,string> attributes;
        string name; // in "from->to" format
};

class DotPrinter {

    using string = std::string;

    public:
        DotPrinter();
        ~DotPrinter();

        void addNode(string name, DotNode* dotNode);
        void addEdge(string name, DotEdge* dotEdge);
        void print() const;

    private:
        std::unordered_map<string,DotNode*> nodes;
        std::unordered_map<string,DotEdge*> edges;
};

} // namespace ppar

#endif // #ifndef DOT_PRINTER_H

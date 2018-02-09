#ifndef PPAR_DEPENDENCE_GRAPH_H
#define PPAR_DEPENDENCE_GRAPH_H

#include <unordered_set>
#include <unordered_map>
#include <functional>

using std::unordered_set;
using std::unordered_map;

namespace ppar 
{

template <typename NODE>
class DependenceGraphNode;

template <typename NODE, typename EDGE>
class DependenceGraphEdge;

template <typename NODE>
struct HashNode {
    size_t operator()(const DependenceGraphNode<NODE>& Node) const {
        return std::hash<NODE*>()(Node.Node);
    }
};

template <typename NODE, typename EDGE>
struct HashEdge {
    size_t operator()(const DependenceGraphEdge<NODE,EDGE>& Edge) const {
        return ( std::hash<EDGE*>()(Edge.Data) ^ 
                 std::hash<NODE*>()(Edge.From) ^ 
                 std::hash<NODE*>()(Edge.To) );
    }
};

template <typename NODE>
class DependenceGraphNode {
    public:
        DependenceGraphNode(NODE* DepNode = nullptr) 
            : Node(DepNode) {}

        DependenceGraphNode(const DependenceGraphNode& DepNode) 
            : Node(DepNode.Node) {}

        ~DependenceGraphNode() {}

        DependenceGraphNode& operator=(const DependenceGraphNode<NODE>& DepNode) {
            Node = DepNode.Node;
            return *this;
        }

        bool operator==(const DependenceGraphNode<NODE>& DepNode) const {
            if (Node == DepNode.Node) {
                return true;
            } else {
                return false;
            }
        }
 
    private:
        NODE* Node;
};

template <typename NODE, typename EDGE>
class DependenceGraphEdge {
    public:
        DependenceGraphEdge()
            : From(nullptr), To(nullptr), Data(nullptr) {}

        DependenceGraphEdge(NODE* From, NODE* To, EDGE* Data = nullptr)
            : From(From), To(To), Data(Data) {}

        ~DependenceGraphEdge() {}
    
        bool operator==(const DependenceGraphEdge<NODE,EDGE>& DepEdge) const {
            if ( (From == DepEdge.From) &&
                 (To == DepEdge.To) &&
                 (Data == DepEdge.Data) ) {
                return true;
            } else {
                return false;
            }
        }

    private:
        EDGE* Data;
        NODE* From;
        NODE* To;
};

/*
 * Graph = {N,E} is represented as two sets: N (nodes) and E (edges)
 */
template <typename NODE, typename EDGE>
class DependenceGraph {
    public:
        DependenceGraph() {
            Nodes.clear();
            Edges.clear();
            Succs.clear();
            Preds.clear();
        }

        ~DependenceGraph() {}
        
        using nodes_set = unordered_set<const DependenceGraphNode<NODE>,HashNode<NODE>>;
        using edges_set = unordered_set<const DependenceGraphEdge<NODE,EDGE>,HashEdge<NODE,EDGE>>;

        using nodes_iterator = typename nodes_set::iterator;
        using const_nodes_iterator = typename nodes_set::const_iterator;
        using dependant_iterator = typename nodes_set::iterator;
        using const_dependant_iterator = typename nodes_set::const_iterator;

        inline void addNode(const NODE* Node);
        inline void addEdge(const NODE* From, const NODE* To, const EDGE* Data = nullptr);

        inline void addPredecessor(const NODE* Node, const NODE* Pred);
        inline void addSuccessor(const NODE* Node, const NODE* Succ);

// two really heavyweight operations (we do not need them for now)
//        inline void removeNode(const NODE* Node);
//        inline void removeEdge(const NODE* From, const NODE* To);

        bool dependsOn(const NODE* NodeA, const NODE* NodeB);

        inline const nodes_set& getDependants(const NODE* Node) const;

        dependant_iterator child_begin(const NODE* Node) 
        { return Succs[DependenceGraphNode<NODE>(Node)].begin(); }

        dependant_iterator child_end(const NODE* Node)
        { return Succs[DependenceGraphNode<NODE>(Node)].end(); }
        
        const_dependant_iterator child_cbegin(const NODE* Node) const
        { return Succs[DependenceGraphNode<NODE>(Node)].cbegin(); }
        
        const_dependant_iterator child_cend(const NODE* Node) const
        { return Succs[DependenceGraphNode<NODE>(Node)].cend(); }

        void clear() { Nodes.clear(); Edges.clear(); Succs.clear(); Preds.clear(); }

        nodes_iterator nodes_begin() { return Nodes.begin(); }
        const_nodes_iterator nodes_cbegin() const { return Nodes.cbegin(); }

        nodes_iterator nodes_end() { return Nodes.end(); }
        const_nodes_iterator nodes_cend() const { return Nodes.cend(); }
    
    private:
        inline bool nodeExists(const NODE* Node) const;

    private:
        nodes_set Nodes; 
        edges_set Edges;
        unordered_map<const DependenceGraphNode<NODE>, nodes_set, HashNode<NODE>> Succs;
        unordered_map<const DependenceGraphNode<NODE>, nodes_set, HashNode<NODE>> Preds;
};

} // namespace ppar

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_H

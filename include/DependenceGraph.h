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
        return std::hash<NODE>()(Node.getNode());
    }
};

template <typename NODE, typename EDGE>
struct HashEdge {
    size_t operator()(const DependenceGraphEdge<NODE,EDGE>& Edge) const {
        return ( std::hash<EDGE>()(Edge.getData()) ^ 
                 std::hash<NODE>()(Edge.getFrom()) ^ 
                 std::hash<NODE>()(Edge.getTo()) );
    }
};

template <typename NODE>
class DependenceGraphNode {
    public:
        DependenceGraphNode(NODE DepNode) 
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
        
        NODE getNode() const { return Node; }

    private:
        NODE Node;
};

template <typename NODE, typename EDGE>
class DependenceGraphEdge {
    public:
        DependenceGraphEdge(NODE From, NODE To, EDGE Data)
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

        EDGE getData() const { return Data; }
        NODE getFrom() const { return From; }
        NODE getTo() const   { return To; }

    private:
        EDGE Data;
        NODE From;
        NODE To;
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

        ~DependenceGraph() {
            Nodes.clear();
            Edges.clear();
            Succs.clear();
            Preds.clear();
        }
        
        using nodes_set = unordered_set<DependenceGraphNode<NODE>,HashNode<NODE>>;
        using edges_set = unordered_set<DependenceGraphEdge<NODE,EDGE>,HashEdge<NODE,EDGE>>;

        using nodes_iterator = typename nodes_set::iterator;
        using const_nodes_iterator = typename nodes_set::const_iterator;
        using dependant_iterator = typename nodes_set::iterator;
        using const_dependant_iterator = typename nodes_set::const_iterator;

        using edges_iterator = typename edges_set::iterator;
        using const_edges_iterator = typename edges_set::const_iterator;

        void addNode(const NODE Node);

        void addEdge(const NODE From, const NODE To, const EDGE Data);

        void addPredecessor(const NODE Node, const NODE Pred);
        void addSuccessor(const NODE Node, const NODE Succ);

// two really heavyweight operations (we do not need them for now)
//        inline void removeNode(const NODE Node);
//        inline void removeEdge(const NODE From, const NODE To);

        bool dependsOn(const NODE NodeA, const NODE NodeB);

        const nodes_set& getDependants(const NODE Node) const;

        dependant_iterator child_begin(const NODE Node) 
        { return Succs[DependenceGraphNode<NODE>(Node)].begin(); }

        dependant_iterator child_end(const NODE Node)
        { return Succs[DependenceGraphNode<NODE>(Node)].end(); }
        
        const_dependant_iterator child_cbegin(const NODE Node) const
        { return Succs[DependenceGraphNode<NODE>(Node)].cbegin(); }
        
        const_dependant_iterator child_cend(const NODE Node) const
        { return Succs[DependenceGraphNode<NODE>(Node)].cend(); }

        void clear() { Nodes.clear(); Edges.clear(); Succs.clear(); Preds.clear(); }

        nodes_iterator nodes_begin() { return Nodes.begin(); }
        const_nodes_iterator nodes_cbegin() const { return Nodes.cbegin(); }

        nodes_iterator nodes_end() { return Nodes.end(); }
        const_nodes_iterator nodes_cend() const { return Nodes.cend(); }

        edges_iterator edges_begin() { return Edges.begin(); }
        const_edges_iterator edges_cbegin() const { return Edges.cbegin(); }

        edges_iterator edges_end() { return Edges.end(); }
        const_edges_iterator edges_cend() const { return Edges.cend(); }

    private:
        bool nodeExists(const NODE Node) const;

    private:
        nodes_set Nodes; 
        edges_set Edges;
        unordered_map<DependenceGraphNode<NODE>, nodes_set, HashNode<NODE>> Succs;
        unordered_map<DependenceGraphNode<NODE>, nodes_set, HashNode<NODE>> Preds;
};

template <typename NODE, typename EDGE>
bool DependenceGraph<NODE,EDGE>::nodeExists(const NODE Node) const {
    const_nodes_iterator node_it = Nodes.find(DependenceGraphNode<NODE>(Node)); 
    if (node_it != Nodes.end()) {
        return true;
    } else {
        return false;
    }
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addNode(const NODE Node) {
    Nodes.insert(DependenceGraphNode<NODE>(Node));
}
        
template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addEdge(const NODE From, const NODE To, const EDGE Data) {
    if (nodeExists(From) && nodeExists(To)) {
        Edges.insert(DependenceGraphEdge<NODE,EDGE>(From, To, Data));
        addPredecessor(To, From);
        addSuccessor(From, To);
    } else {
        // cannot insert and edge between non-existent nodes
//        llvm_unreachable("Cannot add an edge between non-existent nodes");
    }
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addPredecessor(const NODE Node, const NODE Pred) {
    if (nodeExists(Node) && nodeExists(Pred)) {
        Preds[DependenceGraphNode<NODE>(Node)].insert(DependenceGraphNode<NODE>(Pred));
    } else {
        // cannot insert and edge between non-existent nodes
    }
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addSuccessor(const NODE Node, const NODE Succ) {
    if (nodeExists(Node) && nodeExists(Succ)) {
        Succs[DependenceGraphNode<NODE>(Node)].insert(DependenceGraphNode<NODE>(Succ));
    } else {
        // cannot insert and edge between non-existent nodes
    }
} 

template <typename NODE, typename EDGE>
bool DependenceGraph<NODE,EDGE>::dependsOn(const NODE NodeA, const NODE NodeB) {
    if (nodeExists(NodeA) && nodeExists(NodeB)) {
        nodes_set& NodeA_dependants = Succs[DependenceGraphNode<NODE>(NodeA)];
        DependenceGraphNode<NODE> nodeB(NodeB);
        for (DependenceGraphNode<NODE>& dependant  : NodeA_dependants) {
            if( dependant == nodeB) {
                return true;
            }
        }
        return false;
    } else {
        // cannot insert and edge between non-existent nodes
        return false;
    }
}

template <typename NODE, typename EDGE>
const typename DependenceGraph<NODE,EDGE>::nodes_set& ppar::DependenceGraph<NODE,EDGE>::getDependants(const NODE Node) const {
    return Succs[DependenceGraphNode<NODE>(Node)];
}

} // namespace ppar

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_H

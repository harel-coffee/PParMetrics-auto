#ifndef PPAR_DEPENDENCE_GRAPH_H
#define PPAR_DEPENDENCE_GRAPH_H

#include <cstdint>

#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>

namespace ppar {

class Dependence {
    public:
        Dependence() 
            : Flow(false), Anti(false), Output(false), Unknown(true),
              Mem(false), Confused(true), Consistent(false),
              direction(0) {}
        ~Dependence() {}
        
        Dependence(const Dependence& Dep) 
            : Flow(Dep.Flow), Anti(Dep.Anti), Output(Dep.Output), Unknown(Dep.Unknown) {}

        Dependence& operator=(const Dependence& Dep) {
            Flow = Dep.Flow;
            Anti = Dep.Anti;
            Output = Dep.Output;
            Unknown = Dep.Unknown;

            return *this;
        }

        bool isFlow() const { return Flow; }
        bool isAnti() const { return Anti; }
        bool isOutput() const { return Output; }
        bool isUnknown() const { return Unknown; }
        
        bool isMem() const { return Mem; }
        bool isConfused() const { return Confused; }
        bool isConsistent() const { return Consistent; }

        bool Flow;
        bool Anti;
        bool Output;
        bool Unknown;
        
        bool Mem; // obtained from memory dependence analysis
        bool Confused;
        bool Consistent;
        unsigned int direction;
};

template <typename NODE,EDGE>
class DependenceGraphNode;

template <typename NODE, typename EDGE>
class DependenceGraphEdge;

template <typename NODE,EDGE>
struct HashNode {
    size_t operator()(const DependenceGraphNode<NODE,EDGE>& Node) const {
        return std::hash<void*>()(static_cast<void*>(Node.getNode()));
    }
};

template <typename NODE, typename EDGE>
struct HashEdge {
    size_t operator()(const DependenceGraphEdge<NODE,EDGE>& Edge) const {
        return ( std::hash<void*>()(static_cast<void*>(Edge.getData())) ^ 
                 std::hash<void*>()(static_cast<void*>(Edge.getFrom())) ^ 
                 std::hash<void*>()(static_cast<void*>(Edge.getTo())) );
    }
};

template <typename NODE,EDGE>
struct CompareNode {
    bool operator()(const DependenceGraphNode<NODE,EDGE>& NodeA, const DependenceGraphNode<NODE,EDGE>& NodeB) const {
        if (NodeA.ProgramOrder > NodeB.ProgramOrder) {
            return true;
        } else {
            return false;
        }
    }
};

template <typename NODE,EDGE>
class DependenceGraphNode {

    friend class CompareNode<NODE>;
    friend class HashNode<NODE>;
    friend class DepthFirstSearch_iterator<NODE,EDGE>;

    public:
        DependenceGraphNode(NODE DepNode, uint64_t Num) 
            : Node(DepNode), ProgramOrder(Num), NodeColor(WHITE) {}

        DependenceGraphNode(const DependenceGraphNode<NODE,EDGE>& DepNode) 
            : Node(DepNode.Node), ProgramOrder(DepNode.ProgramOrder), NodeColor(DepNode.NodeColor) {}

        ~DependenceGraphNode() {}

        DependenceGraphNode& operator=(const DependenceGraphNode<NODE,EDGE>& DepNode) {
            Node = DepNode.Node;
            ProgramOrder = DepNode.ProgramOrder;
            NodeColor = DepNode.NodeColor;
            return *this;
        }

        bool operator==(const DependenceGraphNode<NODE,EDGE>& DepNode) const {
            if (Node == DepNode.Node) {
                return true;
            } else {
                return false;
            }
        }
        
        NODE getNode() const { return Node; }
        uint64_t getProgramOrder() const { return ProgramOrder; }

    private:
        NODE Node;
        uint64_t ProgramOrder;

        enum class Color {
            GREY,
            WHITE,
            BLACK
        };

        Color NodeColor;
};

template <typename NODE, typename EDGE>
class DependenceGraphEdge {
    public:
        DependenceGraphEdge(NODE From, NODE To, EDGE Data)
            : From(From), To(To), Data(Data) {}

        DependenceGraphEdge(const DependenceGraphEdge& CopyEdge) {
            From = CopyEdge.From;
            To = CopyEdge.To;
            Data = CopyEdge.Data;
        }

        DependenceGraphEdge& operator=(const DependenceGraphEdge& AssignEdge) {
            From = AssignEdge.From;
            To = AssignEdge.To;
            Data = std::move(AssignEdge.Data);
        }

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

template <typename NODE, typename EDGE>
class DepthFirstSearch_iterator;

/*
 * Graph = {N,E} is represented as two sets: N (nodes) and E (edges)
 */
template <typename NODE, typename EDGE>
class DependenceGraph {

    friend class DepthFirstSearch_iterator<NODE,EDGE>;

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
        
        using ordered_nodes_set = std::set<DependenceGraphNode<NODE>,CompareNode<NODE>>;
        using unordered_nodes_set = std::unordered_set<DependenceGraphNode<NODE>,HashNode<NODE>>;
        using unordered_edges_set = std::unordered_set<DependenceGraphEdge<NODE,EDGE>,HashEdge<NODE,EDGE>>;
        using nodes_set = ordered_nodes_set;
        using edges_set = unordered_edges_set;

        using nodes_iterator = typename nodes_set::iterator;
        using const_nodes_iterator = typename nodes_set::const_iterator;

        using edges_iterator = typename edges_set::iterator;
        using const_edges_iterator = typename edges_set::const_iterator;

        using dependant_iterator = typename nodes_set::iterator;
        using const_dependant_iterator = typename nodes_set::const_iterator;

        using dfs_iterator = DepthFirstSearch_iterator<NODE,EDGE>;

        void addNode(const NODE Node);
        void addEdge(const NODE From, const NODE To, EDGE Data);

        void addPredecessor(const NODE Node, const NODE Pred);
        void addSuccessor(const NODE Node, const NODE Succ);

        bool dependsOn(const NODE NodeA, const NODE NodeB);

        const nodes_set& getDependants(const NODE Node) const;

        dfs_iterator dfs_begin() { return DepthFirstSearch_iterator(this, &(*Nodes.begin())); }
        dfs_iterator dfs_end() { return DepthFirstSearch_iterator(this, &(*Nodes.end())); }

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
        std::unordered_map<DependenceGraphNode<NODE>, unordered_nodes_set, HashNode<NODE>> Succs;
        std::unordered_map<DependenceGraphNode<NODE>, unordered_nodes_set, HashNode<NODE>> Preds;
};

template <typename NODE, typename EDGE>
class DependenceGraphSearchInstance {
    public:
        DependenceGraphSearchInstance() {
            State.clear();
        }

        void updateNodeState(DependenceGraphNode<NODE>& Node, std::unique_ptr<NodeProperties> UpdateTo) {
            State[Node] = UpdateTo;
        }

        class NodeProperties {
            public:
                NodeProperties() 
                    : NodeColor(Color::WHITE), TimestampEntry(0), TimestampExit(0), Distance(0) {}

                enum class Color {
                    WHITE,
                    GREY,
                    BLACK
                };

            private:
                Color NodeColor;
                uint64_t TimestampEntry;
                uint64_t TimestampExit;
                uint64_t Distance;
        };

    private:
        std::unordered_map<DependenceGraphNode<NODE>, std::unique_ptr<NodeProperties>, HashNode<NODE>> State;
};

template <typename NODE, typename EDGE>
class DepthFirstSearch_iterator {
    public:    
        DepthFirstSearch_iterator(DependenceGraph<NODE,EDGE>& DepGraph, DependenceGraphNode<NODE>* CurrNode) 
            : DepGraph(DepGraph), CurrentNode(CurrNode) {
            SearchState

        }
        ~DepthFirstSearch_iterator();

        DepthFirstSearch_iterator& operator++() {
            
            for (DependenceGraph<NODE,EDGE>::dependant_iterator dep_it = DepGraph.child_begin(); dep_it != DepGraph.child_end(); dep_it++) {
                if (dep_it->
            }

            return *this;
        }
        
        DependenceGraphNode<NODE>& operator*() {
            return *CurrentNode;    
        }
        
    private:
        DependenceGraph<NODE,EDGE>& DepGraph;
        DependenceGraphNode<NODE>* CurrentNode;
        DependenceGraphSearchInstance<NODE,EDGE> SearchState;
};

} // namespace ppar

#include "DependenceGraph_impl.h"

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_H

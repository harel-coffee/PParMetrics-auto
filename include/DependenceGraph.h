#ifndef PPAR_DEPENDENCE_GRAPH_H
#define PPAR_DEPENDENCE_GRAPH_H

#define DEBUG_TYPE "dep-graph"

#include "ppar_common_includes.h"

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

namespace ppar {

class Dependence;

template <typename NODE, typename EDGE> struct HashNode;
template <typename NODE, typename EDGE> struct HashEdge;

template <typename NODE, typename EDGE> class DependenceGraphNode;
template <typename NODE, typename EDGE> class DependenceGraphEdge;

template <typename NODE, typename EDGE> class DependenceGraph;

class DepthFirstSearch_node_properties;
template <typename NODE, typename EDGE> class DepthFirstSearch_callback;

class Dependence {
    public:
        Dependence() 
            : Data(false), Control(false),
              Flow(false), Anti(false), Output(false),
              Mem(false), Reg(false),
              Confused(true), Consistent(false),
              direction(0),
              Unknown(true) {}
        ~Dependence() {}
        
        Dependence(const Dependence& Dep)
            : Data(Dep.Data), Control(Dep.Control),
              Flow(Dep.Flow), Anti(Dep.Anti), Output(Dep.Output),
              Mem(Dep.Mem), Reg(Dep.Reg),
              Confused(Dep.Confused), Consistent(Dep.Consistent),
              direction(Dep.direction),
              Unknown(Dep.Unknown) {}

        Dependence& operator=(const Dependence& Dep) {
            Data = Dep.Data;
            Control = Dep.Control;

            Flow = Dep.Flow;
            Anti = Dep.Anti;
            Output = Dep.Output;

            Mem = Dep.Mem; 
            Reg = Dep.Reg;

            Confused = Dep.Confused; 
            Consistent = Dep.Consistent;

            direction = Dep.direction;
            
            Unknown = Dep.Unknown;

            return *this;
        }
        
        bool operator==(const Dependence& Dep) {
            if (Dep.isData() && this->isData()) {
                if ( (Dep.isReg() && this->isReg()) ||
                     (Dep.isMem() && this->isMem()) ) {
                    if ( (Dep.isFlow() && this->isFlow()) ||
                         (Dep.isAnti() && this->isAnti()) ||
                         (Dep.isOutput() && this->isOutput()) ) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else if (Dep.isControl() && this->isControl()) {
                return true;
            } else {
                return false;
            }
        }
        
        bool operator!=(const Dependence& Dep) {
            return !(this->operator==(Dep));
        }

        bool isUnknown() const { return Unknown; }

        bool isData() const { return Data; }
        bool isControl() const { return Control; }

        bool isFlow() const { return Flow; }
        bool isAnti() const { return Anti; }
        bool isOutput() const { return Output; }
        
        bool isMem() const { return Mem; }
        bool isReg() const { return Reg; }

        bool isConfused() const { return Confused; }
        bool isConsistent() const { return Consistent; }

        void setUnknown() {
            Data = false;
            Control = false;
            Mem = false;
            Reg = false;
            Unknown = true;
        }

        void setData() {
            Data = true;
            Control = false;
            Unknown = false;
        }

        void setControl() {
            Data = false;
            Control = true;
            Unknown = false;
        }

        void setFlow() { 
            Flow = true;
            Anti = false;
            Output = false;
        }
        
        void setAnti() { 
            Flow = false;
            Anti = true; 
            Output = false;
        }
        
        void setOutput() {
            Flow = false;
            Anti = false; 
            Output = true; 
        }

        void setMem() { 
            Mem = true;
            Reg = false;
        }

        void setReg() { 
            Mem = false;
            Reg = true;
        }

        // type of dependence
        bool Data;
        bool Control;

        // subtype of data dependence
        bool Flow;
        bool Anti;
        bool Output;
        
        // source of dependence 
        bool Mem; // obtained from memory dependence analysis
        bool Reg; // obtained from SSA form
        
        bool Confused;
        bool Consistent;
        unsigned int direction;
        
        bool Unknown;
};

template <typename NODE, typename EDGE>
struct HashNode {
    size_t operator()(const DependenceGraphNode<NODE,EDGE>& Node) const {
        return std::hash<void*>()(static_cast<void*>(Node.getNode()));
    }
};

template <typename NODE, typename EDGE>
struct HashEdge {
    size_t operator()(const DependenceGraphEdge<NODE,EDGE>& Edge) const {
        return ( std::hash<void*>()(static_cast<void*>(Edge.getFrom())) ^ 
                 std::hash<void*>()(static_cast<void*>(Edge.getTo())) );
    }
};

template <typename NODE, typename EDGE>
struct HashNodePair {
    size_t operator()(const std::pair<NODE,NODE>& Pair) const {
        return ( std::hash<void*>()(static_cast<void*>(Pair.first)) ^ 
                 std::hash<void*>()(static_cast<void*>(Pair.second)) );
    }
};

template <typename NODE, typename EDGE>
class DependenceGraphNode {

    friend class HashNode<NODE,EDGE>;
    friend class DependenceGraph<NODE,EDGE>;

    public:
        DependenceGraphNode(NODE DepNode = nullptr) 
            : Node(DepNode) {}

        DependenceGraphNode(const DependenceGraphNode<NODE,EDGE>& DepNode) 
            : Node(DepNode.Node) {}

        ~DependenceGraphNode() {}

        DependenceGraphNode& operator=(const DependenceGraphNode<NODE,EDGE>& DepNode) {
            Node = DepNode.Node;
            return *this;
        }

        bool operator==(const DependenceGraphNode<NODE,EDGE>& DepNode) const {
            if (Node == DepNode.Node) {
                return true;
            } else {
                return false;
            }
        }

        bool operator!=(const DependenceGraphNode<NODE,EDGE>& DepNode) const {
            return !this->operator==(DepNode);
        }

        NODE getNode() const { return Node; }

    private:
        NODE Node;
};

template <typename NODE, typename EDGE>
class DependenceGraphEdge {

    friend class HashEdge<NODE,EDGE>;
    friend class DependenceGraph<NODE,EDGE>;

    public:
        DependenceGraphEdge(NODE From = nullptr, NODE To = nullptr, EDGE Data = nullptr)
            : From(From), To(To), Data(Data), Type(EdgeType::UNDEFINED) {}

        DependenceGraphEdge(const DependenceGraphEdge& CopyEdge) {
            From = CopyEdge.From;
            To = CopyEdge.To;
            Data = CopyEdge.Data;
            Type = CopyEdge.Type;
        }

        DependenceGraphEdge& operator=(const DependenceGraphEdge& AssignEdge) {
            From = AssignEdge.From;
            To = AssignEdge.To;
            Data = AssignEdge.Data;
            Type = AssignEdge.Type;
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
        NODE getTo() const { return To; }
        NODE getType() const { return Type; }

        enum class EdgeType {
            TREE,
            BACK,
            FORWARD,
            CROSS,
            UNDEFINED
        };

    private:
        EDGE Data;
        NODE From;
        NODE To;
        mutable EdgeType Type;
};

/*
 * Graph = {N,E} is represented as two sets: N (nodes) and E (edges)
 */
template <typename NODE, typename EDGE>
class DependenceGraph {

    public:
        DependenceGraph();
        ~DependenceGraph();
        
        using unordered_node_set = std::unordered_set<DependenceGraphNode<NODE,EDGE>,HashNode<NODE,EDGE>>;
        using unordered_edge_set = std::unordered_set<DependenceGraphEdge<NODE,EDGE>,HashEdge<NODE,EDGE>>;
        using node_set = unordered_node_set;
        using edge_set = unordered_edge_set;

        using node_iterator = typename node_set::iterator;
        using const_node_iterator = typename node_set::const_iterator;

        using edge_iterator = typename edge_set::iterator;
        using const_edge_iterator = typename edge_set::const_iterator;

        using dependant_iterator = typename unordered_node_set::iterator;
        using const_dependant_iterator = typename unordered_node_set::const_iterator;

        using dfs_iterator = typename std::vector<DependenceGraphNode<NODE,EDGE>>::iterator;

        static DependenceGraphNode<NODE,EDGE> InvalidNode;

        void addNode(const NODE Node);
        void addEdge(const NODE From, const NODE To, EDGE Data);

        void dfsTraverse(DepthFirstSearch_callback<NODE,EDGE>* VisitorFunc = nullptr) const;

        const unordered_node_set& getDependants(const NODE Node) const;

        dfs_iterator dfs_begin(DepthFirstSearch_callback<NODE,EDGE>* VisitorFunc); 
        dfs_iterator dfs_end(DepthFirstSearch_callback<NODE,EDGE>* VisitorFunc); 

        dependant_iterator child_begin(const NODE Node) 
        { return Succs[DependenceGraphNode<NODE,EDGE>(Node)].begin(); }

        dependant_iterator child_end(const NODE Node)
        { return Succs[DependenceGraphNode<NODE,EDGE>(Node)].end(); }
        
        const_dependant_iterator child_cbegin(const NODE Node) const
        { return Succs[DependenceGraphNode<NODE,EDGE>(Node)].cbegin(); }
        
        const_dependant_iterator child_cend(const NODE Node) const
        { return Succs[DependenceGraphNode<NODE,EDGE>(Node)].cend(); }

        void clear() { Nodes.clear(); Edges.clear(); Succs.clear(); Preds.clear(); }

        node_iterator nodes_begin() { return Nodes.begin(); }
        const_node_iterator nodes_cbegin() const { return Nodes.cbegin(); }

        node_iterator nodes_end() { return Nodes.end(); }
        const_node_iterator nodes_cend() const { return Nodes.cend(); }

        auto edges_begin() { return Edges.begin(); }
        auto edges_cbegin() const { return Edges.cbegin(); }

        auto edges_end() { return Edges.end(); }
        auto edges_cend() const { return Edges.cend(); }

    private:
        const DependenceGraphNode<NODE,EDGE>& nodeExists(const NODE Node) const;
        void addPredecessor(const DependenceGraphNode<NODE,EDGE>& Node, const DependenceGraphNode<NODE,EDGE>& Pred);
        void addSuccessor(const DependenceGraphNode<NODE,EDGE>& Node, const DependenceGraphNode<NODE,EDGE>& Succ);

    private:
        node_set Nodes;
        std::unordered_map<std::pair<NODE,NODE>, edge_set, HashNodePair<NODE,EDGE>> Edges;
        std::unordered_map<DependenceGraphNode<NODE,EDGE>, unordered_node_set, HashNode<NODE,EDGE>> Succs;
        std::unordered_map<DependenceGraphNode<NODE,EDGE>, unordered_node_set, HashNode<NODE,EDGE>> Preds;
        
        // Depth First Search maintenance
        mutable bool DFS_data_valid;
        mutable std::vector<DependenceGraphNode<NODE,EDGE>> DFS_order;
        mutable std::unordered_map<DependenceGraphNode<NODE,EDGE>,std::unique_ptr<DepthFirstSearch_node_properties>,HashNode<NODE,EDGE>> DFS_properties;
};

class DepthFirstSearch_node_properties {
    public:
        DepthFirstSearch_node_properties()
         : TimestampEntry(UINT64_MAX), TimestampExit(UINT64_MAX), Color(NodeColor::WHITE) {}
        ~DepthFirstSearch_node_properties() {}

        DepthFirstSearch_node_properties(DepthFirstSearch_node_properties&& DFS_np) {
            TimestampEntry = DFS_np.TimestampEntry;
            TimestampExit = DFS_np.TimestampExit;
        }

        DepthFirstSearch_node_properties& operator=(DepthFirstSearch_node_properties&& DFS_np) {
            TimestampEntry = DFS_np.TimestampEntry;
            TimestampExit = DFS_np.TimestampExit;
            return *this;
        }

        enum class NodeColor {
            WHITE,
            GREY,
            BLACK
        };
        
        NodeColor Color;
        uint64_t TimestampEntry;
        uint64_t TimestampExit;
};

template <typename NODE, typename EDGE>
class DepthFirstSearch_callback {

    friend class DependenceGraph<NODE,EDGE>;

    public:
        enum class Order {
            PRE,
            POST
        };
    
        DepthFirstSearch_callback(Order CO = Order::POST) : CallOrder(CO) {}

        virtual void operator()() = 0;

    private:
        Order CallOrder;    
};

} // namespace ppar

#include "DependenceGraph_impl.h"

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_H

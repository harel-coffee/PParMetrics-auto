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

#include "llvm/Analysis/DependenceAnalysis.h"

namespace ppar {

class Dependence;

template <typename NODE, typename EDGE> class GraphNode;
template <typename NODE, typename EDGE> class GraphEdge;
template <typename NODE, typename EDGE> class Graph;

class DFS_node_properties;
template <typename NODE, typename EDGE> class DFS_callback;

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

        Dependence& operator+=(const Dependence& Dep){
            if (Dep.isData() && !this->isData()) {
                this->setData();
                if (Dep.isReg()) {
                    this->setReg();
                } else if (Dep.isMem()) {
                    this->setMem();
                }
                if (Dep.isFlow()) {
                    this->setFlow();
                } else if (Dep.isAnti()) {
                    this->setAnti();
                } else if (Dep.isOutput()) {
                    this->setOutput();
                }
            } else if (Dep.isControl() && !this->isControl()) {
                this->setControl();
            } 
            return *this;
        }
        
        Dependence& operator+=(const llvm::Dependence& Dep){
            this->setData();
            this->setMem();
            if (Dep.isFlow()) {
                this->setFlow();
            } else if (Dep.isAnti()) {
                this->setAnti();
            } else if (Dep.isOutput()) {
                this->setOutput();
            }
            return *this;
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
            Flow = false;
            Anti = false;
            Output = false;
            Unknown = true;
        }

        void setData() {
            Data = true;
            Unknown = false;
        }

        void setControl() {
            Control = true;
            Unknown = false;
        }

        void setFlow() { 
            Flow = true;
        }
        
        void setAnti() { 
            Anti = true; 
        }
        
        void setOutput() {
            Output = true; 
        }

        void setMem() { 
            Mem = true;
        }

        void setReg() { 
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
        // types of memory dependences        
        bool Confused;
        bool Consistent;
        unsigned int direction;
        // default 
        bool Unknown;
};

template <typename NODE, typename EDGE>
class GraphNode {

    friend class Graph<NODE,EDGE>;

    public:
        GraphNode(NODE DepNode = nullptr) 
            : Node(DepNode) {}

        GraphNode(const GraphNode<NODE,EDGE>& DepNode) 
            : Node(DepNode.Node) {}

        ~GraphNode() {}

        struct Hash_Node {
            size_t operator()(const GraphNode<NODE,EDGE>& Node) const {
                return std::hash<void*>()(static_cast<void*>(Node.getNode()));
            }
        };
        using hash = Hash_Node;

        GraphNode& operator=(const GraphNode<NODE,EDGE>& DepNode) {
            Node = DepNode.Node;
            return *this;
        }

        bool operator==(const GraphNode<NODE,EDGE>& DepNode) const {
            if (Node == DepNode.Node) {
                return true;
            } else {
                return false;
            }
        }

        bool operator!=(const GraphNode<NODE,EDGE>& DepNode) const {
            return !this->operator==(DepNode);
        }

        NODE getNode() const { return Node; }

    private:
        NODE Node;
};

template <typename NODE, typename EDGE>
class GraphEdge {

    friend class Graph<NODE,EDGE>;

    public:
        GraphEdge(NODE From = nullptr, NODE To = nullptr, EDGE Data = nullptr)
            : From(From), To(To), Data(Data), Type(EdgeType::UNDEFINED) {}

        GraphEdge(const GraphEdge& CopyEdge) {
            From = CopyEdge.From;
            To = CopyEdge.To;
            Data = CopyEdge.Data;
            Type = CopyEdge.Type;
        }

        GraphEdge& operator=(const GraphEdge& AssignEdge) {
            From = AssignEdge.From;
            To = AssignEdge.To;
            Data = AssignEdge.Data;
            Type = AssignEdge.Type;
        }

        ~GraphEdge() {}
 
        struct Hash_Edge {
            size_t operator()(const GraphEdge<NODE,EDGE>& Edge) const {
                return ( std::hash<void*>()(static_cast<void*>(Edge.getFrom())) ^ 
                         std::hash<void*>()(static_cast<void*>(Edge.getTo())) );
            }
        };
        using hash = Hash_Edge;

        bool operator==(const GraphEdge<NODE,EDGE>& DepEdge) const {
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
class Graph {

    public:
        Graph();
        ~Graph();

        // data structures for storing and working with graph nodes and edges
        using unordered_node_set = std::unordered_set<GraphNode<NODE,EDGE>, typename GraphNode<NODE,EDGE>::hash>;
        using unordered_edge_set = std::unordered_set<GraphEdge<NODE,EDGE>, typename GraphEdge<NODE,EDGE>::hash>;
        using node_set = unordered_node_set;
        using edge_set = unordered_edge_set;

        using node_iterator = typename node_set::iterator;
        using const_node_iterator = typename node_set::const_iterator;

        using edge_iterator = typename edge_set::iterator;
        using const_edge_iterator = typename edge_set::const_iterator;

        using dependant_iterator = typename unordered_node_set::iterator;
        using const_dependant_iterator = typename unordered_node_set::const_iterator;

        // Depth-First-Search (DFS) support
        using dfs_iterator = typename std::vector<GraphNode<NODE,EDGE>>::iterator;

        // Strongly Connected Components (SCCs) support
        using sccs_set = std::unordered_map<GraphNode<NODE,EDGE>, Graph<NODE,EDGE>*, typename GraphNode<NODE,EDGE>::hash>;
        using sccs_iterator = typename sccs_set::iterator;
        using const_sccs_iterator = typename sccs_set::const_iterator;

        // returned from various graph methods
        static GraphNode<NODE,EDGE> InvalidNode;

        struct Hash_Graph {
            size_t operator()(const Graph<NODE,EDGE>*& Graph) const {
                return std::hash<void*>()(static_cast<void*>((Graph->getRoot()).getNode()));
            }
        };
        using hash = Hash_Graph;
        
        struct Hash_NodePair {
            size_t operator()(const std::pair<NODE,NODE>& Pair) const {
                return ( std::hash<void*>()(static_cast<void*>(Pair.first)) ^ 
                         std::hash<void*>()(static_cast<void*>(Pair.second)) );
            }
        };
       
        bool operator==(const Graph& G) {
            if (this->Root == G.Root) {
                return true;
            } else {
                return false;
            }
        }

        bool operator!=(const Graph& G) {
            return !(this->operator==(G)); 
        }

        GraphNode<NODE,EDGE> getRoot() const {
            return Root;
        }

        void setRoot(const NODE Node) {
            Root = GraphNode<NODE,EDGE>(Node);
        }

        void addNode(const NODE Node);
        void addEdge(const NODE From, const NODE To, EDGE Data);

        void dfsTraverse(DFS_callback<NODE,EDGE>* VisitorFunc = nullptr) const;
        
        void findSCCs() const;
        void buildComponentGraph() const;

        const unordered_node_set& getDependants(const NODE Node) const;

        dfs_iterator dfs_begin(DFS_callback<NODE,EDGE>* VisitorFunc); 
        dfs_iterator dfs_end(DFS_callback<NODE,EDGE>* VisitorFunc); 

        sccs_iterator sccs_begin()
        { return SCCs.begin(); }
        
        sccs_iterator sccs_end()
        { return SCCs.end(); }

        const_sccs_iterator sccs_cbegin() const 
        { return SCCs.cbegin(); }
        
        const_sccs_iterator sccs_cend() const
        { return SCCs.cend(); }

        dependant_iterator child_begin(const NODE Node) 
        { return Succs[GraphNode<NODE,EDGE>(Node)].begin(); }

        dependant_iterator child_end(const NODE Node)
        { return Succs[GraphNode<NODE,EDGE>(Node)].end(); }
        
        const_dependant_iterator child_cbegin(const NODE Node) const
        { return Succs[GraphNode<NODE,EDGE>(Node)].cbegin(); }
        
        const_dependant_iterator child_cend(const NODE Node) const
        { return Succs[GraphNode<NODE,EDGE>(Node)].cend(); }

        node_iterator nodes_begin() { return Nodes.begin(); }
        const_node_iterator nodes_cbegin() const { return Nodes.cbegin(); }

        node_iterator nodes_end() { return Nodes.end(); }
        const_node_iterator nodes_cend() const { return Nodes.cend(); }

        auto edges_begin() { return Edges.begin(); }
        auto edges_cbegin() const { return Edges.cbegin(); }

        auto edges_end() { return Edges.end(); }
        auto edges_cend() const { return Edges.cend(); }
    
        Graph<NODE,EDGE>* getComponentGraph() const {
            if (ComponentGraph_valid) {
                return ComponentGraph;    
            } else {
                return nullptr;
            }
        }

        auto& getDFS_properties() const {
            return DFS_properties;    
        }

        const auto& getEdges() const {
            return Edges;    
        }

        void clear();

    private:
        const GraphNode<NODE,EDGE>& nodeExists(const NODE Node) const;
        void addPredecessor(const GraphNode<NODE,EDGE>& Node, const GraphNode<NODE,EDGE>& Pred);
        void addSuccessor(const GraphNode<NODE,EDGE>& Node, const GraphNode<NODE,EDGE>& Succ);

    private:
        // Graph representation
        GraphNode<NODE,EDGE> Root;
        node_set Nodes;
        std::unordered_map<std::pair<NODE,NODE>, edge_set, Hash_NodePair> Edges;
        std::unordered_map<GraphNode<NODE,EDGE>, unordered_node_set, typename GraphNode<NODE,EDGE>::hash> Succs;
        std::unordered_map<GraphNode<NODE,EDGE>, unordered_node_set, typename GraphNode<NODE,EDGE>::hash> Preds;

        // Depth First Search (DFS) maintenance
        mutable bool DFS_data_valid;
        mutable std::vector<GraphNode<NODE,EDGE>> DFS_order;
        mutable std::unordered_map< /* key: node */ GraphNode<NODE,EDGE>,
                                    /* value: node properties */ std::unique_ptr<DFS_node_properties>,
                                    /* hash function */ typename GraphNode<NODE,EDGE>::hash> DFS_properties;

        // Strongly Connected Components (SCCs) and Component Graph (CG) maintenance
        mutable bool SCCs_data_valid;
        mutable std::unordered_map< /* key: root node */ GraphNode<NODE,EDGE>, 
                                    /* value: SCC */ Graph<NODE,EDGE>*,
                                    /* hash function */ typename GraphNode<NODE,EDGE>::hash> SCCs;
        mutable std::unordered_map< /* key: node */ GraphNode<NODE,EDGE>, 
                                    /* value: SCC */ Graph<NODE,EDGE>*,
                                    /* hash function */ typename GraphNode<NODE,EDGE>::hash> NodeToSCCs_addr;
        mutable bool ComponentGraph_valid;
        mutable Graph<NODE,EDGE>* ComponentGraph;
};

template <typename NODE, typename EDGE>
EDGE mergeEdges(const typename Graph<NODE,EDGE>::edge_set& EdgeSet);

class DFS_node_properties {
    public:
        DFS_node_properties()
         : TimestampEntry(UINT64_MAX), TimestampExit(UINT64_MAX), Color(NodeColor::WHITE) {}
        ~DFS_node_properties() {}

        DFS_node_properties(DFS_node_properties&& DFS_np) {
            TimestampEntry = DFS_np.TimestampEntry;
            TimestampExit = DFS_np.TimestampExit;
        }

        DFS_node_properties& operator=(DFS_node_properties&& DFS_np) {
            TimestampEntry = DFS_np.TimestampEntry;
            TimestampExit = DFS_np.TimestampExit;
            return *this;
        }

        enum class NodeColor {
            WHITE,
            SILVER, 
            GREY,
            BLACK
        };
        
        NodeColor Color;
        uint64_t TimestampEntry;
        uint64_t TimestampExit;
};

template <typename NODE, typename EDGE>
class DFS_callback {

    friend class Graph<NODE,EDGE>;

    public:
        enum class Order {
            PRE,
            POST
        };
    
        DFS_callback(Order CO = Order::POST) : CallOrder(CO) {}

        virtual void operator()() = 0;

    private:
        Order CallOrder;    
};

} // namespace ppar

#include "Graph_impl.h"

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_H

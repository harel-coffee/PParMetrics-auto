#ifndef PPAR_DEPENDENCE_GRAPH_H
#define PPAR_DEPENDENCE_GRAPH_H

#define DEBUG_TYPE "dep-graph"

#include "ppar_common_includes.h"

#include <cstring>

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"

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
              Direction(llvm::Dependence::DVEntry::NONE),
              Distance(nullptr),
              LoopIndependent(true),
              Scalar(true),
              Unknown(true) {}
        ~Dependence() {}
        
        Dependence(const Dependence& Dep)
            : Data(Dep.Data), Control(Dep.Control),
              Flow(Dep.Flow), Anti(Dep.Anti), Output(Dep.Output),
              Mem(Dep.Mem), Reg(Dep.Reg),
              Confused(Dep.Confused), Consistent(Dep.Consistent),
              Direction(Dep.Direction),
              Distance(Dep.Distance),
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

            Direction = Dep.Direction;
            Distance = Dep.Distance;
            
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

        uint64_t getDirection() const { return Direction; }
        const llvm::SCEV* getDistance() const { return Distance; }
        bool isScalar() const { return Scalar; }
        bool isLoopIndependent() const { return LoopIndependent; }

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
        bool Scalar;
        bool LoopIndependent;
        uint64_t Direction;
        const llvm::SCEV* Distance;
        // default 
        bool Unknown;
};

template <typename NODE, typename EDGE>
class GraphNode; // template parameters must be pointers!

template <typename NODE, typename EDGE>
class GraphNode<NODE*,EDGE*> {
    
    public:
        using node_t = NODE;
        using node_ptr_t = NODE*;
        using edge_t = EDGE;
        using edge_ptr_t = EDGE*;
        
        friend class Graph<NODE*,EDGE*>;
    
        GraphNode(const NODE* DepNode = nullptr) 
         : Node(DepNode) {}

        GraphNode(const GraphNode<NODE*,EDGE*>& DepNode) 
         : Node(DepNode.Node) {}

        ~GraphNode() {}

        struct Hash_Node {
            size_t operator()(const GraphNode<NODE*,EDGE*>& Node) const {
                return std::hash<const void*>()(static_cast<const void*>(Node.getNode()));
            }
        };
        using hash = Hash_Node;

        GraphNode& operator=(const GraphNode<NODE*,EDGE*>& DepNode) {
            Node = DepNode.Node;
            return *this;
        }

        bool operator==(const GraphNode<NODE*,EDGE*>& DepNode) const {
            if (Node == DepNode.Node) {
                return true;
            } else {
                return false;
            }
        }

        bool operator!=(const GraphNode<NODE*,EDGE*>& DepNode) const {
            return !this->operator==(DepNode);
        }

        const NODE* getNode() const { return Node; }

    private:
        const NODE* Node;
};

template <typename NODE, typename EDGE>
bool operator==(const GraphNode<NODE*,EDGE*>& NodeA, const GraphNode<NODE*,EDGE*>& NodeB) {
    if (NodeA.getNode() == NodeB.getNode()) {
        return true;
    } else {
        return false;
    }
}

template <typename NODE, typename EDGE>
class GraphEdge;

template <typename NODE, typename EDGE>
class GraphEdge<NODE*,EDGE*> {

    public:
        using node_t = NODE;
        using node_ptr_t = NODE*;
        using edge_t = EDGE;
        using edge_ptr_t = EDGE*;

        friend class Graph<NODE*,EDGE*>;

        GraphEdge(const NODE* From = nullptr, const NODE* To = nullptr, EDGE* Data = nullptr)
            : From(From), To(To), Data(Data), Type(EdgeType::UNDEFINED) {}

        GraphEdge(NODE* From = nullptr, NODE* To = nullptr, EDGE* Data = nullptr)
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
            size_t operator()(const GraphEdge<NODE*,EDGE*>& Edge) const {
                return ( std::hash<const void*>()(static_cast<const void*>(Edge.getFrom())) ^ 
                         std::hash<const void*>()(static_cast<const void*>(Edge.getTo())) );
            }
        };
        using hash = Hash_Edge;

        bool operator==(const GraphEdge<NODE*,EDGE*>& DepEdge) const {
            if ( (From == DepEdge.From) &&
                 (To == DepEdge.To) &&
                 (Data == DepEdge.Data) ) {
                return true;
            } else {
                return false;
            }
        }

        EDGE* getData() const { return Data; }
        const NODE* getFrom() const { return From; }
        const NODE* getTo() const { return To; }

        enum class EdgeType {
            TREE,
            BACK,
            FORWARD,
            CROSS,
            UNDEFINED
        };
        
        EdgeType getType() const { return Type; }

    private:
        EDGE* Data;
        const NODE* From;
        const NODE* To;
        mutable EdgeType Type;
};

/*
 * Graph = {N,E} is represented as two sets: N (nodes) and E (edges)
 */

template <typename NODE, typename EDGE>
class Graph;

template <typename NODE, typename EDGE>
class Graph<NODE*,EDGE*> {

    public:
        using node_t = NODE;
        using node_ptr_t = NODE*;
        using edge_t = EDGE;
        using edge_ptr_t = EDGE*;

        Graph(llvm::Pass* GraphPass = nullptr, const llvm::Function* F = nullptr, const llvm::Loop* L = nullptr, const Graph<NODE*,EDGE*>* Parent = nullptr);
        ~Graph();

        // data structures for storing and working with graph nodes and edges
        using unordered_node_set = std::unordered_set<GraphNode<NODE*,EDGE*>, typename GraphNode<NODE*,EDGE*>::hash>;
        using unordered_edge_set = std::unordered_set<GraphEdge<NODE*,EDGE*>, typename GraphEdge<NODE*,EDGE*>::hash>;
        using node_set = unordered_node_set;
        using edge_set = unordered_edge_set;

        using node_iterator = typename node_set::iterator;
        using const_node_iterator = typename node_set::const_iterator;

        using edge_iterator = typename edge_set::iterator;
        using const_edge_iterator = typename edge_set::const_iterator;

        using dependant_iterator = typename unordered_node_set::iterator;
        using const_dependant_iterator = typename unordered_node_set::const_iterator;

        // Depth-First-Search (DFS) support
        using dfs_iterator = typename std::vector<GraphNode<NODE*,EDGE*>>::iterator;

        // Strongly Connected Components (SCCs) support
        using sccs_set = std::unordered_map<GraphNode<NODE*,EDGE*>, 
                                            Graph<NODE*,EDGE*>*, 
                                            typename GraphNode<NODE*,EDGE*>::hash>;
        using sccs_iterator = typename sccs_set::iterator;
        using const_sccs_iterator = typename sccs_set::const_iterator;

        // returned from various graph methods
        static GraphNode<NODE*,EDGE*> InvalidNode;
        static GraphEdge<NODE*,EDGE*> InvalidEdge;
        static edge_set InvalidEdgeSet;

        struct Hash_Graph {
            size_t operator()(const Graph<NODE*,EDGE*>*& Graph) const {
                return std::hash<const void*>()(static_cast<const void*>((Graph->getRoot()).getNode()));
            }
        };
        using hash = Hash_Graph;
        
        struct Hash_NodePair {
            size_t operator()(const std::pair<const NODE*,const NODE*>& Pair) const {
                return ( std::hash<const void*>()(static_cast<const void*>(Pair.first)) ^ 
                         std::hash<const void*>()(static_cast<const void*>(Pair.second)) );
            }
        };

        bool operator==(const Graph<NODE*,EDGE*>& G) const {
            if ( (this->Func == G.Func) &&
                 (this->Loop == G.Loop) &&
                 (this->GraphPass == G.GraphPass)) {
                if (this->Root == G.Root) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        bool operator!=(const Graph<NODE*,EDGE*>& G) const {
            return !(this->operator==(G)); 
        }

        const Graph<NODE*,EDGE*>* getParent() const {
            return ParentGraph;
        }

        GraphNode<NODE*,EDGE*> getRoot() const {
            return Root;
        }

        void setRoot(const NODE* Node) {
            Root = GraphNode<NODE*,EDGE*>(Node);
        }

        void addNode(const NODE* Node);
        void addEdge(const NODE* From, const NODE* To, EDGE* Data);
        
        GraphNode<NODE*,EDGE*> getNode(const NODE* Node) const {
            const_node_iterator node_it = Nodes.find(GraphNode<NODE*,EDGE*>(Node)); 
            if (node_it != Nodes.end()) {
                return *node_it;
            } else {
                return Graph<NODE*,EDGE*>::InvalidNode;
            }
        }

        edge_set& getEdgeSet(const NODE* From, const NODE* To) {
            auto it = Edges.find(std::make_pair(From,To)); 
            if (it != Edges.end()) {
                return *it;
            } else {
                return Graph<NODE*,EDGE*>::InvalidEdgeSet;
            }
        }

        bool nodeHasIncomingEdges(GraphNode<NODE*,EDGE*> Node) {
            auto preds_it = Preds.find(Node);
            if (preds_it != Preds.end()) {
                if (!Preds[Node].empty()) {
                    return true; 
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        void dfsTraverse(DFS_callback<NODE*,EDGE*>* VisitorFunc = nullptr) const;
        
        void findSCCs() const;
        void buildComponentGraph() const;

        const unordered_node_set& getDependants(const NODE* Node) const;

        dfs_iterator dfs_begin(DFS_callback<NODE*,EDGE*>* VisitorFunc); 
        dfs_iterator dfs_end(DFS_callback<NODE*,EDGE*>* VisitorFunc); 

        sccs_iterator sccs_begin()
        { return SCCs.begin(); }
        
        sccs_iterator sccs_end()
        { return SCCs.end(); }

        const_sccs_iterator sccs_cbegin() const 
        { return SCCs.cbegin(); }
        
        const_sccs_iterator sccs_cend() const
        { return SCCs.cend(); }

        dependant_iterator child_begin(const NODE* Node) 
        { return Succs[GraphNode<NODE*,EDGE*>(Node)].begin(); }

        dependant_iterator child_end(const NODE* Node)
        { return Succs[GraphNode<NODE*,EDGE*>(Node)].end(); }
        
        const_dependant_iterator child_cbegin(const NODE* Node) const
        { return Succs[GraphNode<NODE*,EDGE*>(Node)].cbegin(); }
        
        const_dependant_iterator child_cend(const NODE* Node) const
        { return Succs[GraphNode<NODE*,EDGE*>(Node)].cend(); }

        node_iterator nodes_begin() { return Nodes.begin(); }
        const_node_iterator nodes_cbegin() const { return Nodes.cbegin(); }

        node_iterator nodes_end() { return Nodes.end(); }
        const_node_iterator nodes_cend() const { return Nodes.cend(); }

        auto edges_begin() { return Edges.begin(); }
        auto edges_cbegin() const { return Edges.cbegin(); }

        auto edges_end() { return Edges.end(); }
        auto edges_cend() const { return Edges.cend(); }
  
        // 
        // Queries related to computed Strongly Connected Components (SCCs), 
        // Component Graph (CG), etc.
        //
        bool isSCCsDataValid() const { return SCCs_data_valid; }
        bool isDFSDataValid() const { return DFS_data_valid; }
        bool isComponentGraphDataValid() const { return CG_data_valid; }

        Graph<NODE*,EDGE*>* nodeToSCC(NODE* Node) const {
            if (SCCs_data_valid) {
                return NodeToSCCs_addr[GraphNode<NODE*,EDGE*>(Node)];
            } else {
                return nullptr;
            }
        }

        GraphNode<NODE*,EDGE*> getSCCRoot(NODE* Node) const {
            if (SCCs_data_valid) {
                return NodeToSCCRoot[GraphNode<NODE*,EDGE*>(Node)];
            } else {
                return InvalidNode;
            }
        }

        Graph<NODE*,EDGE*>* getComponentGraph() const {
            if (CG_data_valid) {
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

        const auto& getPredecessors() const {
            return Preds;    
        }

        void clear();
 
        // Queries related to general graph properties (nodes and edges number, etc.)
        //

        // Graph size queries
        uint64_t getNodesNumber() const { return Nodes.size(); }
        uint64_t getEdgesNumber() const { return Edges.size(); }
        uint64_t getMultiEdgesNumber() const { 
            uint64_t Num = 0;
            for (const auto it = Edges.cbegin(); it = Edges.cend(); ++it) {
                Num += (it->second).size();
            }
            return Num; 
        }
        uint64_t getEdgesNumber_Type(typename GraphEdge<NODE*,EDGE*>::EdgeType Type) const { 
            uint64_t Num = 0;
            for (const auto it = Edges.cbegin(); it = Edges.cend(); ++it) {
                edge_set& EdgeSet = it->second;
                for (auto& edge : EdgeSet) {
                    if (edge.getType() == Type) {
                        Num++;
                    }
                }
            }
            return Num; 
        }

    private:
        const GraphNode<NODE*,EDGE*>& nodeExists(const NODE* Node) const;
        void addPredecessor(const GraphNode<NODE*,EDGE*>& Node, const GraphNode<NODE*,EDGE*>& Pred);
        void addSuccessor(const GraphNode<NODE*,EDGE*>& Node, const GraphNode<NODE*,EDGE*>& Succ);

    private:
        
        // Function the graph is built on
        const llvm::Function* Func;
        // Loop the graph is built on
        const llvm::Loop* Loop;
        // Pass, building the graph
        llvm::Pass* GraphPass;
        // SCCs must refer to the parent graph they have been computed on
        const Graph<NODE*,EDGE*>* ParentGraph;

        /* 
         * Graph representation 
         */

        GraphNode<NODE*,EDGE*> Root;
        node_set Nodes;
        std::unordered_map<std::pair<const NODE*,const NODE*>, edge_set, Hash_NodePair> Edges;
        std::unordered_map<GraphNode<NODE*,EDGE*>, unordered_node_set, typename GraphNode<NODE*,EDGE*>::hash> Succs;
        std::unordered_map<GraphNode<NODE*,EDGE*>, unordered_node_set, typename GraphNode<NODE*,EDGE*>::hash> Preds;

        /* 
         * Graph's computable data 
         */

        // Depth First Search (DFS) maintenance
        mutable bool DFS_data_valid;
        mutable std::vector<GraphNode<NODE*,EDGE*>> DFS_order;
        mutable std::unordered_map< /* key: node */ GraphNode<NODE*,EDGE*>,
                                    /* value: node properties */ std::unique_ptr<DFS_node_properties>,
                                    /* hash function */ typename GraphNode<NODE*,EDGE*>::hash> DFS_properties;

        // Strongly Connected Components (SCCs) and Component Graph (CG) maintenance
        mutable bool SCCs_data_valid;
        mutable std::unordered_map< /* key: root node */ GraphNode<NODE*,EDGE*>, 
                                    /* value: SCC */ Graph<NODE*,EDGE*>*,
                                    /* hash function */ typename GraphNode<NODE*,EDGE*>::hash> SCCs;
        mutable std::unordered_map< /* key: node */ GraphNode<NODE*,EDGE*>, 
                                    /* value: SCC */ Graph<NODE*,EDGE*>*,
                                    /* hash function */ typename GraphNode<NODE*,EDGE*>::hash> NodeToSCCs_addr;
        mutable std::unordered_map< /* key: node */ GraphNode<NODE*,EDGE*>, 
                                    /* value: SCC */ GraphNode<NODE*,EDGE*>,
                                    /* hash function */ typename GraphNode<NODE*,EDGE*>::hash> NodeToSCCRoot;
        mutable bool CG_data_valid;
        mutable Graph<NODE*,EDGE*>* ComponentGraph;
};

template<typename NODE,typename EDGE>
struct EdgeCopier;

template<typename NODE,typename EDGE>
struct EdgeCopier<NODE*,EDGE*> {
    public:
        EdgeCopier(llvm::Pass* Pass) 
         : GraphPass(Pass) {}
                
        EDGE* operator()(const GraphEdge<NODE*,EDGE*>& Edge) {
            EDGE* Dep = new EDGE();
            *Dep = *(Edge.getData());
            return Dep;
        }

        llvm::Pass* GraphPass;
};

template<typename NODE>
struct EdgeCopier<NODE*,llvm::Dependence*> {
    public:
        EdgeCopier(llvm::Pass* GraphPass) {
            if (llvm::DependenceAnalysisWrapperPass* DepAnalysisPass = GraphPass->getAnalysisIfAvailable<llvm::DependenceAnalysisWrapperPass>()) {
                DepAnalysisWrapperPass = DepAnalysisPass;
            } else {
                llvm_unreachable("Cannot create Graph<NODE,llvm::Dependence*>::EdgeCopier since llvm::DependenceAnalysisWrapperPass is not available!");
            }
        }

        llvm::Dependence* operator()(const GraphEdge<NODE*,llvm::Dependence*>& Edge) {
            if (llvm::Dependence* Dep = ((DepAnalysisWrapperPass->getDI()).depends(const_cast<NODE*>(Edge.getFrom()), 
                                                                                   const_cast<NODE*>(Edge.getTo()), true)).release()) {
                return Dep;
            } else {
                llvm_unreachable("Graph<NODE,llvm::Dependence*>::EdgeCopier failed to copy the edge!");
            }
        }
 
        llvm::DependenceAnalysisWrapperPass* DepAnalysisWrapperPass;
};

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

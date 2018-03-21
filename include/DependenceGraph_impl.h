#ifndef PPAR_DEPENDENCE_GRAPH_IMPL_H
#define PPAR_DEPENDENCE_GRAPH_IMPL_H

#include <memory>
#include <stack>

namespace ppar {

template <typename NODE, typename EDGE>
DependenceGraph<NODE,EDGE>::DependenceGraph() {
    Nodes.clear();
    Edges.clear();
    Succs.clear();
    Preds.clear();

    DFS_data_valid = false;
    DFS_order.clear();
    DFS_properties.clear();
}

template <typename NODE, typename EDGE>
DependenceGraph<NODE,EDGE>::~DependenceGraph() {

    for (edge_iterator edge_it = Edges.begin(); edge_it != Edges.end(); edge_it++) {
        if (edge_it->Data != nullptr) {
            delete edge_it->Data;
        }
    }

    Nodes.clear();
    Edges.clear();
    Succs.clear();
    Preds.clear();

    DFS_data_valid = false;
    DFS_order.clear();
    DFS_properties.clear();
}

template <typename NODE, typename EDGE>
DependenceGraphNode<NODE,EDGE> DependenceGraph<NODE,EDGE>::InvalidNode = DependenceGraphNode<NODE,EDGE>();

template <typename NODE, typename EDGE>
typename DependenceGraph<NODE,EDGE>::dfs_iterator DependenceGraph<NODE,EDGE>::dfs_begin(DepthFirstSearch_callback<NODE,EDGE>* DFSCallback) {
    if (!DFS_data_valid) {
        dfsTraverse();
    }
    return DFS_order.begin();
}
        
template <typename NODE, typename EDGE>
typename DependenceGraph<NODE,EDGE>::dfs_iterator DependenceGraph<NODE,EDGE>::dfs_end(DepthFirstSearch_callback<NODE,EDGE>* DFSCallback) {
    if (!DFS_data_valid) {
        dfsTraverse();
    }
    return DFS_order.end(); 
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addNode(const NODE Node, uint64_t ProgramOrder) {
    Nodes.insert(DependenceGraphNode<NODE,EDGE>(Node, ProgramOrder));
}
        
template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addEdge(const NODE From, const NODE To, const EDGE Data) {
    const DependenceGraphNode<NODE,EDGE>& FromNode = nodeExists(From);
    const DependenceGraphNode<NODE,EDGE>& ToNode = nodeExists(To);

    if ( (FromNode != InvalidNode) &&
         (ToNode != InvalidNode) ) {
        Edges.insert(DependenceGraphEdge<NODE,EDGE>(From, To, Data));
        addPredecessor(ToNode, FromNode);
        addSuccessor(FromNode, ToNode);
    } else {
        // cannot insert and edge between non-existent nodes
//        llvm_unreachable("Cannot add an edge between non-existent nodes");
    }
}

template <typename NODE, typename EDGE>
const DependenceGraphNode<NODE,EDGE>& DependenceGraph<NODE,EDGE>::nodeExists(const NODE Node) const {
    const_node_iterator node_it = Nodes.find(DependenceGraphNode<NODE,EDGE>(Node)); 
    if (node_it != Nodes.end()) {
        return *node_it;
    } else {
        return DependenceGraph<NODE,EDGE>::InvalidNode;
    }
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addPredecessor(const DependenceGraphNode<NODE,EDGE>& Node, const DependenceGraphNode<NODE,EDGE>& Pred) {
    Preds[DependenceGraphNode<NODE,EDGE>(Node)].insert(DependenceGraphNode<NODE,EDGE>(Pred));
}

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addSuccessor(const DependenceGraphNode<NODE,EDGE>& Node, const DependenceGraphNode<NODE,EDGE>& Succ) {
    Succs[DependenceGraphNode<NODE,EDGE>(Node)].insert(DependenceGraphNode<NODE,EDGE>(Succ));
} 

template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::dfsTraverse(DepthFirstSearch_callback<NODE,EDGE>* VisitorFunc) const {
    // SearchSet - is the set of all not yet discovered nodes
    // (initially equal to the set of all graph nodes - Nodes
    node_set SearchSet(Nodes);
    // Intermediate data structure - nodes reside here in between white (not yet discovered) 
    // and black (all successors have been completely processed) states - grey state;
    node_set GreySet;
    // Stack - is used for the iterative DFS algorithm
    std::stack<DependenceGraphNode<NODE,EDGE>> Stack;
    // CurrentTime - DFS tracks traversal time 
    uint64_t CurrentTime = 0;

    while (!SearchSet.empty()) { // we still have undiscovered nodes
        node_iterator node_it = SearchSet.begin(); // we take nodes in their linear program order
        DependenceGraphNode<NODE,EDGE> Node = *node_it;
        SearchSet.erase(node_it); // node is discovered
        Stack.push(Node);
        while (!Stack.empty()) {
            DependenceGraphNode<NODE,EDGE> CurrentNode(Stack.top()); // process the node at the top of the stack
            const DependenceGraph<NODE,EDGE>::unordered_node_set& Successors = Succs.at(CurrentNode);
            bool NodeIsProcessed = true; // we finished with the node and all its successors
            for (typename unordered_node_set::const_iterator succ_it = Successors.cbegin(); succ_it != Successors.cend(); succ_it++) {
                auto edge_it = Edges.find(DependenceGraphEdge<NODE,EDGE>(CurrentNode.Node, succ_it->Node));
                if (edge_it == Edges.end()) {
                    // error inconsistent dependence graph data structure
                }
                if (SearchSet.find(*succ_it) != SearchSet.end()) { // WHITE 
                    // successor has not been discovered yet ->
                    // hence the node is not yet completely processed
                    NodeIsProcessed = false;
                    Stack.push(DependenceGraphNode<NODE,EDGE>(*succ_it));
                    GreySet.insert(DependenceGraphNode<NODE,EDGE>(*succ_it));
                    SearchSet.erase(DependenceGraphNode<NODE,EDGE>(*succ_it)); // mark successor as discovered
                    // since this successor hasn't been visited yet, 
                    // we mark correcponding edge as of tree edge type
                    edge_it->Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::TREE;
                } else if (GreySet.find(*succ_it) != SearchSet.end()) { // GREY
                    edge_it->Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::BACK;
                } else { // BLACK
                    if (DFS_properties[CurrentNode]->TimestampEntry < CurrentTime) {
                        edge_it->Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::FORWARD;
                    } else {
                        edge_it->Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::CROSS;
                    }
                }
            }
            if (NodeIsProcessed) { // either successor set is empty or
                // all of them have been visited
                Stack.pop();
                GreySet.erase(CurrentNode); // mark node as black
                DFS_properties[CurrentNode]->TimestampExit = CurrentTime;
                if ( VisitorFunc != nullptr &&
                     (VisitorFunc->CallOrder == DepthFirstSearch_callback<NODE,EDGE>::Order::POST)) {
                    VisitorFunc->operator()();
                }
            } else {
                // node has just been discovered -> set discovery time
                DFS_properties[CurrentNode] = std::make_unique<DepthFirstSearch_node_properties>();
                DFS_properties[CurrentNode]->TimestampEntry = CurrentTime;
                // build linear DFS-ordered vector
                DFS_order.push_back(CurrentNode);

                if ( VisitorFunc != nullptr &&
                     (VisitorFunc->CallOrder == DepthFirstSearch_callback<NODE,EDGE>::Order::PRE) ) {
                    VisitorFunc->operator()();
                }
            }
            CurrentTime++;
        }
    }

    DFS_data_valid = true;
}

template <typename NODE, typename EDGE>
const typename DependenceGraph<NODE,EDGE>::unordered_node_set& ppar::DependenceGraph<NODE,EDGE>::getDependants(const NODE Node) const {
    return Succs[DependenceGraphNode<NODE,EDGE>(Node)];
}

} // namespace ppar

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_IMPL_H

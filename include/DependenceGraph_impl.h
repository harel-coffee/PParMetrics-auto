#ifndef PPAR_DEPENDENCE_GRAPH_IMPL_H
#define PPAR_DEPENDENCE_GRAPH_IMPL_H

#include "ppar_common_includes.h"

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

    for (auto edge_it = Edges.begin(); edge_it != Edges.end(); edge_it++) {
        edge_set& EdgeSet = edge_it->second;
        for (auto& edge : EdgeSet) {
            if (edge.Data != nullptr) {
                delete edge.Data;
            }
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
void DependenceGraph<NODE,EDGE>::addNode(const NODE Node) {
    Nodes.insert(DependenceGraphNode<NODE,EDGE>(Node));
}
        
template <typename NODE, typename EDGE>
void DependenceGraph<NODE,EDGE>::addEdge(const NODE From, const NODE To, const EDGE Data) {
    const DependenceGraphNode<NODE,EDGE>& FromNode = nodeExists(From);
    const DependenceGraphNode<NODE,EDGE>& ToNode = nodeExists(To);

    if ( (FromNode != InvalidNode) &&
         (ToNode != InvalidNode) ) {
        Edges[std::make_pair(From,To)].insert(DependenceGraphEdge<NODE,EDGE>(From, To, Data));
        addPredecessor(ToNode, FromNode);
        addSuccessor(FromNode, ToNode);
    } else {
        // cannot insert and edge between non-existent nodes
        llvm_unreachable("Cannot add an edge between non-existent nodes");
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
    // SearchSet - is the set of all not yet discovered nodes (whites)
    // (initially equal to the set of all graph nodes - Nodes
    node_set SearchSet(Nodes);
    // Stack - is used for the iterative DFS algorithm
    std::stack<DependenceGraphNode<NODE,EDGE>> Stack;
    // CurrentTime - DFS tracks traversal time 
    uint64_t CurrentTime = 0;
  
    DEBUG(
        llvm::dbgs() << "[[" << CurrentTime << "] new DFS traversal]\n";
    );

    DFS_properties.clear();
    for (const_node_iterator node_it = Nodes.cbegin(); node_it != Nodes.cend(); node_it++) {
        DFS_properties[*node_it] = std::make_unique<DepthFirstSearch_node_properties>();
    }
 
    while (!SearchSet.empty()) { // we still have undiscovered (white) nodes
        Stack.push(*SearchSet.begin());

        DEBUG(
            std::string str;
            llvm::raw_string_ostream rso(str);
            ((SearchSet.begin())->getNode())->print(rso);
            llvm::dbgs() << "[[" << CurrentTime << "] new DFS tree: root(" << str << ")]\n";
        );

        while (!Stack.empty()) {
            bool NodeIsProcessed = true; // we are done with the node and all its successors
            DependenceGraphNode<NODE,EDGE> CurrentNode(Stack.top());
            DepthFirstSearch_node_properties* Node_props = DFS_properties[CurrentNode].get();

            DEBUG(
                std::string str;
                llvm::raw_string_ostream rso(str);
                (CurrentNode.getNode())->print(rso);

                if (Node_props->Color == DepthFirstSearch_node_properties::NodeColor::WHITE) {
                    llvm::dbgs() << "[" << CurrentTime << "] taking the node off the stack: WHITE " << str << "\n";
                } else if (Node_props->Color == DepthFirstSearch_node_properties::NodeColor::GREY) {
                    llvm::dbgs() << "[" << CurrentTime << "] taking the node off the stack: GREY " << str << "\n";
                } else {
                    llvm::dbgs() << "[" << CurrentTime << "] taking the node off the stack: BLACK " << str << "\n";
                }
            );

            if (Node_props->Color == DepthFirstSearch_node_properties::NodeColor::WHITE) {
                Node_props->Color = DepthFirstSearch_node_properties::NodeColor::GREY;
                DFS_properties[CurrentNode]->TimestampEntry = CurrentTime;
                SearchSet.erase(CurrentNode); // node is discovered
                if ( VisitorFunc != nullptr &&
                     (VisitorFunc->CallOrder == DepthFirstSearch_callback<NODE,EDGE>::Order::PRE) ) {
                    VisitorFunc->operator()();
                }

                DEBUG(
                    std::string str;
                    llvm::raw_string_ostream rso(str);
                    (CurrentNode.getNode())->print(rso);
                    llvm::dbgs() << "[" << CurrentTime << "] discovered new node WHITE->GREY: " << str << "\n";
                );
            }

            const auto it = Succs.find(CurrentNode);
            if (it != Succs.end()) {
                const DependenceGraph<NODE,EDGE>::unordered_node_set& Successors = Succs.at(CurrentNode);
                for (typename unordered_node_set::const_iterator succ_it = Successors.cbegin(); succ_it != Successors.cend(); succ_it++) {
                    // mark edge with the corresponding edge type
                    const edge_set& EdgesSet = Edges.at(std::make_pair(CurrentNode.Node, succ_it->Node));
                    if (EdgesSet.empty()) {
                        llvm_unreachable("Inconsistent dependence graph data structure: edge does not exist");
                    }
                    typename DependenceGraphEdge<NODE,EDGE>::EdgeType Type;
                    DepthFirstSearch_node_properties::NodeColor SuccNodeColor = DFS_properties[DependenceGraphNode<NODE,EDGE>(*succ_it)]->Color;
                    if (SuccNodeColor == DepthFirstSearch_node_properties::NodeColor::WHITE) {
                        // successor has not been discovered yet ->
                        // hence the node is not yet completely processed
                        NodeIsProcessed = false;
                        Stack.push(DependenceGraphNode<NODE,EDGE>(*succ_it));
                        // since this successor hasn't been visited yet, 
                        // we mark corresponding edge as of tree edge type
                        Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::TREE;
        
                        DEBUG(
                            std::string str;
                            llvm::raw_string_ostream rso(str);
                            ((*succ_it).getNode())->print(rso);
                            llvm::dbgs() << "[" << CurrentTime << "] found successor: WHITE " << str << "\n";
                        );

                    } else if (SuccNodeColor == DepthFirstSearch_node_properties::NodeColor::GREY) {
                        Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::BACK;

                        DEBUG(
                            std::string str;
                            llvm::raw_string_ostream rso(str);
                            ((*succ_it).getNode())->print(rso);
                            llvm::dbgs() << "[" << CurrentTime << "] found successor: GREY " << str << "\n";
                        );

                    } else { // BLACK
                        if (DFS_properties[CurrentNode]->TimestampEntry < CurrentTime) {
                            Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::FORWARD;
                        } else {
                            Type = DependenceGraphEdge<NODE,EDGE>::EdgeType::CROSS;
                        }

                        DEBUG(
                            std::string str;
                            llvm::raw_string_ostream rso(str);
                            ((*succ_it).getNode())->print(rso);
                            llvm::dbgs() << "[" << CurrentTime << "] found successor: BLACK " << str << "\n";
                        );

                    }
                    for (auto& edge : EdgesSet) {
                        edge.Type = Type;
                    }
                }
            } else {
                DEBUG(
                    std::string str;
                    llvm::raw_string_ostream rso(str);
                    (CurrentNode.getNode())->print(rso);
                    llvm::dbgs() << "node has no successors: " << str << "\n";
                );
            }

            if (NodeIsProcessed) {
                // we won't ever return to it in the current traversal
                Stack.pop();
                DFS_order.push_back(CurrentNode);
                DFS_properties[CurrentNode]->TimestampExit = CurrentTime;
                DFS_properties[CurrentNode]->Color = DepthFirstSearch_node_properties::NodeColor::BLACK;
                if ( VisitorFunc != nullptr &&
                     (VisitorFunc->CallOrder == DepthFirstSearch_callback<NODE,EDGE>::Order::POST)) {
                    VisitorFunc->operator()();
                }

                DEBUG(
                    std::string str;
                    llvm::raw_string_ostream rso(str);
                    (CurrentNode.getNode())->print(rso);
                    llvm::dbgs() << "[" << CurrentTime << "] node is processed: -> BLACK " << str << "\n";
                );

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

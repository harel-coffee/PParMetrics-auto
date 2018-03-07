#ifndef PPAR_DEPENDENCE_GRAPH_IMPL_H
#define PPAR_DEPENDENCE_GRAPH_IMPL_H

#include <memory>

namespace ppar {

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
            if (dependant == nodeB) {
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

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_IMPL_H

#ifndef PPAR_DEPENDENCE_GRAPH_ITERATOR_H
#define PPAR_DEPENDENCE_GRAPH_ITERATOR_H

#include <cstdint>

#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>

namespace ppar {

class NodeSearchProperties {
    public:
        NodeSearchProperties() 
         : NodeColor(color::white), TimestampEntry(0), TimestampExit(0), Distance(UINT64_MAX) {}

        enum class Color {
            WHITE,
            GREY,
            BLACK
        };

        Color NodeColor;
        uint64_t TimestampEntry;
        uint64_t TimestampExit;
        uint64_t Distance;
};

template <typename NODE, typename EDGE>
class DependenceGraphSearchInstance {
    public:
        DependenceGraphSearchInstance(DependenceGraph<NODE<EDGE>& DG) : DepGraph(DG) {
            State.clear();
            Pred.clear();
            for (node_iterator node_it = DepGraph.nodes_begin(); node_it != DepGraph.nodes_end(); node_it++) {
                State[*node_it] = make_unique(new NodeSearchProperties());            
            }
            
            for (node_iterator node_it = DepGraph.nodes_begin(); node_it != DepGraph.nodes_end(); node_it++) {
                Pred[*node_it] = DependenceGraphNode();            
            }
        }

        void updateNodeState(DependenceGraphNode<NODE>& Node, std::unique_ptr<NodeProperties> UpdateTo) {
            State[Node] = UpdateTo;
        }

        std::unique_ptr<NodeProperties>& operator[](DependenceGraphNode<NODE> Node) {
            return State[Node];
        }

        DependenceGraphNode<NODE,EDGE>& predecessor(DependenceGraphNode<NODE,EDGE> Node) {
            return Pred[Node];
        }

    private:
        DependenceGraphNode<NODE,EDGE>& DepGraph;
        std::unordered_map<DependenceGraphNode<NODE,EDGE>, std::unique_ptr<NodeProperties>, HashNode<NODE,EDGE>> State;
        std::unordered_map<DependenceGraphNode<NODE,EDGE>, DependenceGraphNode<NODE,EDGE>, HashNode<NODE,EDGE>> Pred;
};

template <typename NODE, typename EDGE>
class DepthFirstSearch_callback {

    friend class class DepthFirstSearch_iterator<NODE,EDGE>;

    public:
        DepthFirstSearch_callback() : CallOrder(Order::POST) {}
        DepthFirstSearch_callback(Order CO) : CallOrder(CO) {}
    
        enum class Order {
            PRE,
            POST
        };

        virtual void operator() = 0;

    private:
        Order CallOrder;    
};

template <typename NODE, typename EDGE>
class DepthFirstSearch_iterator {
    public:    
        DepthFirstSearch_iterator(DependenceGraph<NODE,EDGE>& DepGraph, DependenceGraphNode<NODE,EDGE> CurrNode, DepthFirstSearch_callback* C = nullptr) 
            : DepGraph(DepGraph), CurrentNode(CurrNode), PredNode(DependenceGraphNode()), SearchState(DepGraph), CurrentTime(0), Callback(C) {}
        ~DepthFirstSearch_iterator();

        DepthFirstSearch_iterator& operator++() {
            
            if (Callback != nullptr &&
                Callback.CallOrder == DepthFirstSearch_callback::Order::PRE) { 
                (*Callback)();
            }

            for (auto dep_it = DepGraph.child_begin(*CurrentNode); dep_it != DepGraph.child_end(*CurrentNode); dep_it++) {
                NodeProperties* NodeProps = SearchState[*dep_it];
                if (NodeProps->NodeColor == NodeSearchProperties::Color::WHITE) {
                    NodeProps->NodeColor = NodeSearchProperties::Color::GREY;
                    NodeProps->TimestampEntry = CurrentTime++;
                    SearchState.predecessor(*dep_it) = CurrentNode;
                    CurrentNode = *dep_it;
                    return *this;
                }
            }
            NodeProperties* NodeProps = SearchState[CurrentNode];
            NodeProps->NodeColor = NodeSearchProperties::Color::BLACK;
            NodeProps->TimestampExit = CurrentTime++;
            
            if (Callback != nullptr &&
                Callback.CallOrder == DepthFirstSearch_callback::Order::POST) { 
                (*Callback)();
            }

            CurrentNode = SearchState.predecessor(CurrentNode);
            return *this;
        }
        
        DependenceGraphNode<NODE,EDGE>& operator*() {
            return CurrentNode;    
        }

        DependenceGraphSearchInstance<NODE,EDGE>& getSearchState() { return SearchState; }
        
    private:
        uint64_t CurrentTime;
        DependenceGraph<NODE,EDGE>& DepGraph;
        DependenceGraphNode<NODE,EDGE> CurrentNode;
        DependenceGraphSearchInstance<NODE,EDGE> SearchState;
        DepthFirstSearch_callback* Callback;
};

} // namespace ppar

#endif // #ifndef PPAR_DEPENDENCE_GRAPH_ITERATOR_H

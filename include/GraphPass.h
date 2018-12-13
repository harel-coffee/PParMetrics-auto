/*
 * GraphPass<NODE*,EDGE*,PASS> template
 *
 * As this pass runs on a given function, it builds function's dependence graph 
 * and all dependence graphs of the function's loops.
 *
 * To create a custom pass provide a class specialization with exact NODE and 
 * EDGE types. Third PASS argument is used to distinguish different graphs with 
 * the same NODE and EDGE types, and is not supposed to encapsulate any 
 * heavyweight functionality. PASS must conform to the GraphPass_iface.
 *
 */

#ifndef PPAR_GRAPH_PASS_H
#define PPAR_GRAPH_PASS_H

#include "ppar_common_includes.h"
#include "Graph.h"
#include "FunctionLoopInfo.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Analysis/LoopInfo.h"

namespace ppar {

template <typename NODE, typename EDGE, typename PASS>
struct GraphPass;

template <typename NODE, typename EDGE, typename PASS>
struct GraphPass<NODE*,EDGE*,PASS> : public llvm::FunctionPass {
    public:
        static bool skipInstruction(const llvm::Instruction* Inst);

    public:
        static char ID;
        GraphPass();

        using LoopToDependenceGraph = std::unordered_map<const llvm::Loop*,std::unique_ptr<Graph<NODE*,EDGE*>>>;
        using loop_graph_iterator = typename LoopToDependenceGraph::iterator;
        using const_loop_graph_iterator = typename LoopToDependenceGraph::const_iterator;

        static Graph<NODE*,EDGE*> InvalidGraph;

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

        Graph<NODE*,EDGE*>& getFunctionGraph();
        Graph<NODE*,EDGE*>& getLoopGraph(const llvm::Loop*);

        loop_graph_iterator loops_begin() { return LG.begin(); }
        const_loop_graph_iterator loops_cbegin() const { return LG.cbegin(); }

        loop_graph_iterator loops_end() { return LG.end(); }
        const_loop_graph_iterator loops_cend() const { return LG.cend(); }

        void allocateGraphs(llvm::Function& F);

    private:
        std::unique_ptr<Graph<NODE*,EDGE*>> G;
        std::unordered_map<const llvm::Loop*,std::unique_ptr<Graph<NODE*,EDGE*>>> LG;
};

} // namespace ppar

#endif // ifndef PPAR_GRAPH_PASS_H

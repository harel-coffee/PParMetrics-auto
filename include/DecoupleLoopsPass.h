#ifndef PPAR_DECOUPLE_LOOPS_PASS_H
#define PPAR_DECOUPLE_LOOPS_PASS_H

#include "ppar_common_includes.h"
#include "Graph.h"
#include "GraphPass.h"
#include "GraphPasses.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "decouple-loops"

namespace ppar {

class LoopDecoupleInfo {

    public:

        using DependenceGraph = Graph<llvm::Instruction*,ppar::Dependence*>;

        LoopDecoupleInfo() {
            Iterator.clear();
            Payload.clear();
            SCCs.clear();
        }

        ~LoopDecoupleInfo() {
            Iterator.clear();
            Payload.clear();
            SCCs.clear();
        }

        void addSCC(DependenceGraph* S) { SCCs.insert(S); }
        void addPayloadSCC(DependenceGraph* S) { Payload.insert(S); }
        void addIteratorSCC(DependenceGraph* S) { Iterator.insert(S); }

        bool nodeBelongsToIterator(const llvm::Instruction* Node);
        bool nodeBelongsToPayload(const llvm::Instruction* Node);

        std::unordered_set<DependenceGraph*>& getIterator() { return Iterator; }
        std::unordered_set<DependenceGraph*>& getPayload() { return Payload; }
        std::unordered_set<DependenceGraph*>& getSCCs() { return SCCs; }

        bool SCCBelongsToIterator(DependenceGraph* DG) {
            auto it = Iterator.find(DG);
            if (it != Iterator.end()) {
                return true;
            } else {
                return false;
            }
        }

        bool SCCBelongsToPayload(DependenceGraph* DG) {
            auto it = Payload.find(DG);
            if (it != Payload.end()) {
                return true;
            } else {
                return false;
            }
        }

    private:
        std::unordered_set<DependenceGraph*> Iterator;
        std::unordered_set<DependenceGraph*> Payload;
        std::unordered_set<DependenceGraph*> SCCs; // Payload + Iterator
};

struct DecoupleLoopsPass : public llvm::FunctionPass {

    public:
        using DependenceGraph = Graph<llvm::Instruction*,ppar::Dependence*>;
        using PDGPass = GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>;

        static char ID;
        DecoupleLoopsPass();
        ~DecoupleLoopsPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

        std::unique_ptr<LoopDecoupleInfo> decoupleLoops(DependenceGraph& G);
        
        bool allocateLoopDecoupleInfo(llvm::Function& F);

        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& getDecoupleLoopsInfo_func() {
            return DecoupleLoopsInfo_func;
        }

        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& getDecoupleLoopsInfo_loop() {
            return DecoupleLoopsInfo_loop;
        }

    private:
        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>> DecoupleLoopsInfo_func; // derived out of function scope dependence graph
        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>> DecoupleLoopsInfo_loop; // derived out of single loop scope dependence graph
};

} // namespace ppar

#endif // #ifndef PPAR_DECOUPLE_LOOPS_PASS_H

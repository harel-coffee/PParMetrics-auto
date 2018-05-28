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

        void addSCC(const DependenceGraph* S) { SCCs.insert(S); }
        void addPayloadSCC(const DependenceGraph* S) { Payload.insert(S); }
        void addIteratorSCC(const DependenceGraph* S) { Iterator.insert(S); }

        const std::unordered_set<const DependenceGraph*>& getIterator() const { return Iterator; }
        const std::unordered_set<const DependenceGraph*>& getPayload() const { return Payload; }
        const std::unordered_set<const DependenceGraph*>& getSCCs() const { return SCCs; }

    private:
        std::unordered_set<const DependenceGraph*> Iterator;
        std::unordered_set<const DependenceGraph*> Payload;
        std::unordered_set<const DependenceGraph*> SCCs; // Payload + Iterator
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

        const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& getDecoupleLoopsInfo_func() {
            return DecoupleLoopsInfo_func;
        }

        const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& getDecoupleLoopsInfo_loop() {
            return DecoupleLoopsInfo_loop;
        }

    private:
        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>> DecoupleLoopsInfo_func; // derived out of function scope dependence graph
        std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>> DecoupleLoopsInfo_loop; // derived out of single loop scope dependence graph
        std::vector<const llvm::Loop*> FunctionLoops;
        std::unordered_map<const llvm::Loop*, std::string> LoopAddrToName;
};

} // namespace ppar

#endif // #ifndef PPAR_DECOUPLE_LOOPS_PASS_H

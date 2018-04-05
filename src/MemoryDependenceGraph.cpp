#include "MemoryDependenceGraph.h"

#include "llvm/IR/InstIterator.h"
using namespace llvm;

#include <memory>
using namespace std;

char ppar::MemoryDependenceGraphPass::ID = 0;
RegisterPass<ppar::MemoryDependenceGraphPass> MDGRegister("mdg", "Build in-memory Memory Dependence Graph of a function");

namespace ppar {

MemoryDependenceGraphPass::MemoryDependenceGraphPass() 
    : FunctionPass(ID) {}

void MemoryDependenceGraphPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<DependenceAnalysisWrapperPass>();
}

StringRef MemoryDependenceGraphPass::getPassName() const { 
    return "Memory Dependence Graph Pass"; 
}

void MemoryDependenceGraphPass::releaseMemory() { 
    MDG.clear(); 
}

DependenceGraph<Instruction*,llvm::Dependence*>& MemoryDependenceGraphPass::getMDG() { 
    return MDG;
}

bool MemoryDependenceGraphPass::runOnFunction(Function &F) {
    
    DependenceInfo& DI = Pass::getAnalysis<DependenceAnalysisWrapperPass>().getDI();

    vector<Instruction*> MemRefs;

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (isa<StoreInst>(*I) || isa<LoadInst>(*I)) {
            MemRefs.push_back(&*I);
        }
    }

    for (auto Inst : MemRefs) {
        MDG.addNode(Inst);
    }
    
    for (auto SrcI : MemRefs) {
        for (auto DstI : MemRefs) {
            if (llvm::Dependence* D = (DI.depends(SrcI, DstI, true)).release()) {
                if (D->isFlow() || D->isAnti()) MDG.addEdge(SrcI, DstI, D);
            }
        }
    }
    
    return false;
}

} // namespace ppar

#include "MemoryDependenceGraph.h"

#include "llvm/IR/InstIterator.h"
using namespace llvm;

#include <memory>
using namespace std;

char ppar::MemoryDependenceGraphPass::ID = 0;
RegisterPass<ppar::MemoryDependenceGraphPass> MDGRegister("mdg", "Print MDG of a function to 'dot' file");

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

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (isa<StoreInst>(*I) || isa<LoadInst>(*I)) {
            Instruction* Inst = &*I;
            MDG.addNode(Inst);
        }
    }
  
    for (inst_iterator SrcI = inst_begin(F), SrcE = inst_end(F); SrcI != SrcE; ++SrcI) {
        if (isa<StoreInst>(*SrcI) || isa<LoadInst>(*SrcI)) {
            for (inst_iterator DstI = SrcI, DstE = inst_end(F); DstI != DstE; ++DstI) {
                if (isa<StoreInst>(*DstI) || isa<LoadInst>(*DstI)) {
                    if (llvm::Dependence* D = (DI.depends(&*SrcI, &*DstI, true)).release()) {
                        if (D->isFlow() || D->isAnti()) MDG.addEdge(&*SrcI, &*DstI, D);
                    }
                }
            }
        }
    }
    
    return false;
}

} // namespace ppar

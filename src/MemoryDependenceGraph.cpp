
#include "MemoryDependenceGraph.h"

#include "llvm/IR/InstIterator.h"

#include <memory>
using std::unique_ptr;

namespace ppar {

bool MemoryDependenceGraphPass::runOnFunction(Function &F) {
    DependenceInfo& DI = Pass::getAnalysis<DependenceAnalysisWrapperPass>().getDI();

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (isa<StoreInst>(*I) || isa<LoadInst>(*I)) {
            Instruction *Inst = &*I;
            MDG.addNode(Inst);
        }
    }
  
    for (inst_iterator SrcI = inst_begin(F), SrcE = inst_end(F); SrcI != SrcE; ++SrcI) {
        if (isa<StoreInst>(*SrcI) || isa<LoadInst>(*SrcI)) {
            for (inst_iterator DstI = SrcI, DstE = inst_end(F); DstI != DstE; ++DstI) {
                if (isa<StoreInst>(*DstI) || isa<LoadInst>(*DstI)) {
                    if (Dependence* D = (DI.depends(&*SrcI, &*DstI, true)).get()) {
                        if (D->isFlow() || D->isAnti()) MDG.addEdge(&*SrcI, &*DstI, D);
                    }
                }
            }
        }
    }
    
    return false;
}

} // namespace ppar

char ppar::MemoryDependenceGraphPass::ID = 0;
RegisterPass<ppar::MemoryDependenceGraphPass> MDGRegister("mdg", "Print MDG of a function to 'dot' file");

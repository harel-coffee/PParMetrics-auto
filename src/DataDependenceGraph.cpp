#include "DataDependenceGraph.h"

#include "llvm/IR/User.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Casting.h"

using namespace std;
using namespace llvm;

char ppar::DataDependenceGraphPass::ID = 0;
RegisterPass<ppar::DataDependenceGraphPass> DDGRegister("ddg", "Build Data Dependence Graph");

namespace ppar {

DataDependenceGraphPass::DataDependenceGraphPass() 
    : FunctionPass(ID) {}

void DataDependenceGraphPass::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
}

StringRef DataDependenceGraphPass::getPassName() const { 
    return "Data Dependence Graph"; 
}

void DataDependenceGraphPass::releaseMemory() { 
    for (ppar::Dependence* dep : DepsPool) {
        delete dep;
    }
    DepsPool.clear();
    DDG.clear();
}

const DependenceGraph<Instruction*,ppar::Dependence*>& DataDependenceGraphPass::getDDG() const { 
    return DDG; 
}

bool DataDependenceGraphPass::runOnFunction(Function &F) {

    using llvm::User;
    using llvm::inst_iterator;
    using llvm::dyn_cast;

    // add nodes
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
        Instruction* Inst = &*I;
        DDG.addNode(Inst);
    }

    // add edges
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        // every user of an instruction is its user
        for (User* U : I->users()) {
            ppar::Dependence* Dep = new ppar::Dependence();
            DepsPool.push_back(Dep);
            Dep->Flow = true;
            if (Instruction* Inst = dyn_cast<Instruction>(U)) {
                DDG.addEdge(&*I, Inst, Dep);
            }
        }
    }

    return false;
}

} // namespace ppar

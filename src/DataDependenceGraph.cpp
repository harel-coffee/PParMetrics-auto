#include "DataDependenceGraph.h"

#include "llvm/IR/User.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Casting.h"
using namespace llvm;

#include <memory>
using namespace std;

char ppar::DataDependenceGraphPass::ID = 0;
RegisterPass<ppar::DataDependenceGraphPass> DDGRegister("ddg", "Build in-memory Data Dependence Graph of a function");

namespace ppar {

DataDependenceGraphPass::DataDependenceGraphPass() : FunctionPass(ID) {}

void DataDependenceGraphPass::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
}

StringRef DataDependenceGraphPass::getPassName() const { 
    return "Data Dependence Graph"; 
}

void DataDependenceGraphPass::releaseMemory() {
}

Graph<Instruction*,ppar::Dependence*>& DataDependenceGraphPass::getDDG() { 
    return DDG; 
}

bool DataDependenceGraphPass::runOnFunction(Function &F) {

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
            Dep->setData();
            Dep->setFlow();
            Dep->setReg();

            if (Instruction* Inst = dyn_cast<Instruction>(U)) {
                DDG.addEdge(&*I, Inst, Dep);
            }
        }
    }

    return false;
}

} // namespace ppar

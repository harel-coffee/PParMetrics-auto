#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassAnalysisSupport.h"
using namespace llvm;

#include "MemoryDependenceGraph.h"
using namespace ppar;

namespace {

struct MDGPrinter : public FunctionPass {
    static char ID;
    MDGPrinter() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override; 
    void getAnalysisUsage(AnalysisUsage &AU) const override;
};

}

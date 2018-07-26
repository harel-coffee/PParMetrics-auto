#include "FunctionLoopInfo.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
using namespace llvm;

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

RegisterPass<ppar::FunctionLoopInfoPass> FunctionLoopInfoPassRegister(
    "function-loop-info",
    "Wrapper pass around llvm::LoopInfo (builds a list of function loops and establishes a naming scheme across the PPar tool)"
);

namespace ppar {

char FunctionLoopInfoPass::ID = 0;

FunctionLoopInfoPass::FunctionLoopInfoPass() 
 : FunctionPass(ID) {

    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        llvm::dbgs() << "FunctionLoopInfoPass::FunctionLoopInfoPass()\n";
    );

    FunctionLoopLists.clear();
    FunctionLoopNameMappings.clear();
}

FunctionLoopInfoPass::~FunctionLoopInfoPass() {

    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        llvm::dbgs() << "FunctionLoopInfoPass::~FunctionLoopInfoPass()\n";
    );

    FunctionLoopLists.clear();
    FunctionLoopNameMappings.clear();
}

void FunctionLoopInfoPass::releaseMemory() {
    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        llvm::dbgs() << "FunctionLoopInfoPass::releaseMemory()\n";
    );
}

void FunctionLoopInfoPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

llvm::StringRef FunctionLoopInfoPass::getPassName() const { 
    return "FunctionLoopInfoPass"; 
}

bool FunctionLoopInfoPass::runOnFunction(llvm::Function& F) {
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();

    DEBUG_WITH_TYPE("ppar-pass-pipeline",
        llvm::dbgs() << "FunctionLoopInfoPass::runOnFunction()\n";
    );

    // create corresponding entries for a new function F
    FunctionLoopList& LList = FunctionLoopLists[&F];
    LoopNames& LNames = FunctionLoopNameMappings[&F];

    if (LI.empty()) {
        llvm::outs() << "Function Loop Info Pass:\n";
        llvm::outs() << "\tNo top-level loops in the function!\n";
        return false; 
    }

    // line up all the function loops sequantially in a list
    std::queue<const llvm::Loop*> LoopsQueue;
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const llvm::Loop* TopLevelL = *loop_it;
        LoopsQueue.push(TopLevelL);
        while(!LoopsQueue.empty()) {
            const llvm::Loop* CurrentLoop = LoopsQueue.front();
            LList.push_back(CurrentLoop);
            LoopsQueue.pop();
            for (auto sub_loop_it = CurrentLoop->begin(); sub_loop_it != CurrentLoop->end(); ++sub_loop_it) {
                LoopsQueue.push(*sub_loop_it);
            }
        }
    }

    int i = 0;
    for (const Loop* L : LList) {
        // assign a name to the loop 
        string LoopName = F.getName().str() + ".depth_" + std::to_string(L->getLoopDepth()) + ".loop_" + std::to_string(i++);
        LNames[L] = LoopName;

        DEBUG_WITH_TYPE("ppar-function-loop-info",
            std::string str;
            llvm::raw_string_ostream rso(str);
            llvm::dbgs() << "new loop identified: " + LoopName + "\n";
            L->dump();
            llvm::dbgs() << (L->getName()).str() << "\n";
            llvm::dbgs() << "\n";
        );

        LNames[L] = LoopName;
    }

    return false;
}

} // namespace ppar

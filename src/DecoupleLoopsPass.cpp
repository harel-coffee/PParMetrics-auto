#include "DecoupleLoopsPass.h"

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
#include <memory>
#include <map>
#include <set>
using namespace std;

RegisterPass<ppar::DecoupleLoopsPass> DecoupleLoopsRegister(
    "decouple-loops",
    "Identify the iterator and workload instructions of the loops in a function"
);

namespace ppar {

char DecoupleLoopsPass::ID = 0;

DecoupleLoopsPass::DecoupleLoopsPass() 
 : FunctionPass(ID) {
    LoopsDepInfo.clear();
}

void DecoupleLoopsPass::releaseMemory() {

    DEBUG(
        llvm::dbgs() << "[debug] DecoupleLoopsPass::releaseMemory()\n";
    );

    LoopsDepInfo.clear();
}

void DecoupleLoopsPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<PDGPass>();
}

llvm::StringRef DecoupleLoopsPass::getPassName() const { 
    return "Decouple Loops Pass"; 
}

bool DecoupleLoopsPass::runOnFunction(llvm::Function& F) {
    
    if (F.isDeclaration()) return false;

    // get computed information about function loops and dependencies
    const DependenceGraph& PDG = (getAnalysis<PDGPass>()).getFunctionGraph();
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    std::vector<const Loop*> FunctionLoops;
    std::map<const Loop*, std::string> LoopAddrToName;

    llvm::outs() << "Decouple loops function pass: " + F.getName() + "\n";

    if (LI.empty()) {
        llvm::outs() << "\tNo top-level loops in the function!\n";
        return false; 
    }
        
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const Loop* TopLevelL = *loop_it;
        FunctionLoops.push_back(TopLevelL);
        for (auto sub_loop_it = TopLevelL->begin(); sub_loop_it != TopLevelL->end(); ++sub_loop_it) {
            const Loop* SubL = *sub_loop_it;
            FunctionLoops.push_back(SubL);
        }
    }

    int i = 0;
    for (const Loop* L : FunctionLoops) {
        string LoopName = "loop" + std::to_string(i);

        DEBUG(
            std::string str;
            llvm::raw_string_ostream rso(str);
            llvm::dbgs() << "new loop identified: " + LoopName + "\n";
            L->dump();
            llvm::dbgs() << "\n";
        );

        LoopAddrToName[L] = LoopName;
        LoopsDepInfo[L] = std::make_unique<LoopDependenceInfo>();
    }

    if (!PDG.isSCCsDataValid() || !PDG.isComponentGraphDataValid()) {
        PDG.findSCCs();
        PDG.buildComponentGraph();
    }
    const DependenceGraph* CGraph = PDG.getComponentGraph();

    // collect the information 
    for (Function::iterator bb_it = F.begin(); bb_it != F.end(); ++bb_it) {
        Loop* InnermostL = LI.getLoopFor(&(*bb_it));
        if (InnermostL != nullptr) {
            // current basic block is within the loop, so all SCCs
            // formed out of that BB instructions belong to the loop
            for (BasicBlock::iterator inst_it = bb_it->begin(); inst_it != bb_it->end(); ++inst_it) {
                DependenceGraph* SCC = PDG.nodeToSCC(&(*inst_it));
                LoopsDepInfo[InnermostL]->addSCC(SCC);

                DEBUG(
                    std::string str;
                    llvm::raw_string_ostream rso(str);
                    (SCC->getRoot()).getNode()->print(rso);
                    llvm::dbgs() <<  LoopAddrToName[InnermostL] << ": added SCC(" << SCC << ") root: " << str << "\n";
                );

                bool outsideLoop = true;
                const auto& Preds = CGraph->getPredecessors();
                auto preds_it = Preds.find(SCC->getRoot());
                if (preds_it != Preds.end()) {
                    for (DependenceGraph::const_node_iterator pred_it = preds_it->second.cbegin(); pred_it != preds_it->second.cend(); pred_it++) {
                        Loop* Parent = LI.getLoopFor((pred_it->getNode())->getParent());
                        if (Parent == InnermostL) {
                            outsideLoop = false;
                        }
                    }
                }

                if (outsideLoop) {
                    DEBUG(
                        llvm::dbgs() << "SCC(" << SCC << ") is the iterator of the " << LoopAddrToName[InnermostL] + "\n";
                    );
                    LoopsDepInfo[InnermostL]->addIteratorSCC(SCC);
                } else {
                    DEBUG(
                        llvm::dbgs() << "added SCC(" << SCC << ") is the workload of the " << LoopAddrToName[InnermostL] + "\n";
                    );
                    LoopsDepInfo[InnermostL]->addPayloadSCC(SCC);
                }
            }
        } 
    }

    return false;
}

} // namespace ppar


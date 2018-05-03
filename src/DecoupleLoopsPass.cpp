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
    ScalarCode.clear(); 
}

void DecoupleLoopsPass::releaseMemory() {}

void DecoupleLoopsPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<PDGPass>();
}

llvm::StringRef DecoupleLoopsPass::getPassName() const { 
    return "Decouple Loops Pass"; 
}

bool DecoupleLoopsPass::runOnFunction(llvm::Function& F) {
    // get computed information about function loops and dependencies
    const DependenceGraph& PDG = (Pass::getAnalysis<PDGPass>()).getGraph();
    const LoopInfo& LI = (Pass::getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    
    DEBUG(
        llvm::dbgs() << "[ Decouple Loops Function Pass ] " + F.getName() + "\n";
    );

    if (!PDG.isSCCsDataValid() || !PDG.isComponentGraphDataValid()) {
        PDG.findSCCs();
        PDG.buildComponentGraph();
    }

    std::vector<const DependenceGraph*> functionSCCs;
    DependenceGraph* CGraph = PDG.getComponentGraph();

    for (DependenceGraph::const_sccs_iterator sccs_it = PDG.sccs_cbegin(); sccs_it != PDG.sccs_cend(); ++sccs_it) {
        functionSCCs.push_back(sccs_it->second);
    }

    // prepare LoopDependenceInfo for information collection
    int i = 0;
    std::map<const Loop*,std::string> LoopAddrToName;
    for (Loop* L : LI) {
        LoopAddrToName[L] = std::string("loop") + std::to_string(i);
        i++;
        
        DEBUG(
            std::string str;
            llvm::raw_string_ostream rso(str);
            llvm::dbgs() << "new loop identified: " + LoopAddrToName[L] +"\n";
            L->dump();
            llvm::dbgs() << "\n";
        );

        LoopsDepInfo[L] = std::make_unique<LoopDependenceInfo>();
    }

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
                if (!CGraph->nodeHasIncomingEdges(SCC->getRoot())) {
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
        } else {
            for (BasicBlock::iterator inst_it = bb_it->begin(); inst_it != bb_it->end(); ++inst_it) {
                DependenceGraph* SCC = PDG.nodeToSCC(&(*inst_it));
                ScalarCode.push_back(SCC);
                DEBUG(
                    llvm::dbgs() << "SCC(" << SCC << ") is the scalar code\n";
                );
            }
        }
    }

    return false;
}

} // namespace ppar


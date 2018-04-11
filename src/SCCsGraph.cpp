#include "SCCsGraph.h"
#include "ProgramDependenceGraph.h"

char ppar::SCCsGraphPass::ID = 0;
static RegisterPass<ppar::SCCsGraphPass> SCCsGraph("sccs", "splits the PDG into its Strongly Connected Components (SCCs)");

namespace ppar {
   
bool SCCsGraphPass::runOnFunction(Function& F) {
    const Graph<Instruction*,ppar::Dependence*>& pdg = Pass::getAnalysis<ProgramDependenceGraphPass>().getPDG();

    pdg.findSCCs();
    pdg.buildComponentGraph();

    return false;
}

SCCsGraphPass::SCCsGraphPass() : FunctionPass(ID) {}

void SCCsGraphPass::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<ProgramDependenceGraphPass>();
}

} // namespace ppar

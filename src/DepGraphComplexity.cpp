#include "DepGraphComplexity.h"

STATISTIC(DepGraphComplexity, "Calculates cyclomatic complexity (CC) of the given function's dependence graph");

char ppar::DepGraphComplexity::ID = 0;
static RegisterPass<ppar::DepGraphComplexity> CCmplx("ccomp", "computes cyclomatic complexity of the given function");

namespace ppar {
   
class DGComplexity : public DepthFirstSearch_callback<Instruction*,ppar::Dependence*> {
    public:
        DGComplexity() : DepthFirstSearch_callback<Instruction*,ppar::Dependence*>(DepthFirstSearch_callback::Order::POST), Complexity(0) {}
        void operator()() override { Complexity++; }
        uint64_t Complexity;
};

bool DepGraphComplexity::runOnFunction(Function& F) {
    const DependenceGraph<Instruction*,ppar::Dependence*>& pdg = Pass::getAnalysis<ProgramDependenceGraphPass>().getPDG();
    DGComplexity DGComp;

    pdg.dfsTraverse(&DGComp);

    outs() << "Program Dependence Graph Cyclomatic Complexity is " << DGComp.Complexity << "!\n";

    return false;
}

DepGraphComplexity::DepGraphComplexity() : FunctionPass(ID) {}

void DepGraphComplexity::getAnalysisUsage(AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<ProgramDependenceGraphPass>();
}

} // namespace ppar

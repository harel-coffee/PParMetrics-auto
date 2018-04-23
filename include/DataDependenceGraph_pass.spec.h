#ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H
#define PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

namespace ppar {

template<>
char GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::ID = 0;

template <>
void GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
}

template <>
llvm::StringRef GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::getPassName() const {
    return DataDependenceGraphPass::getPassName();
}

template <>
bool GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::runOnFunction(Function& F) {
    // add nodes
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
        Instruction* Inst = &*I;
        getGraph().addNode(Inst);
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
                getGraph().addEdge(&*I, Inst, Dep);
            }
        }
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_DATA_DEPENDENCE_GRAPH_PASS_SPEC_H

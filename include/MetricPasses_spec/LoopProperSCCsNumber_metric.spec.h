#ifndef PPAR_LOOP_PROPER_SCCS_NUMBER_H
#define PPAR_LOOP_PROPER_SCCS_NUMBER_H

namespace ppar {

template<>
char MetricPass<ppar::LoopProperSCCsNumber>::ID = 0;

template <>
void MetricPass<ppar::LoopProperSCCsNumber>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopProperSCCsNumber>::runOnFunction(Function& F) {
    // get access to information passes, metric passes depend on
    GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>& PDGPass =
        Pass::getAnalysis<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
    DecoupleLoopsPass& DecLoopsPass = Pass::getAnalysis<DecoupleLoopsPass>(); 
    LoopInfoWrapperPass& LIPass = (getAnalysis<LoopInfoWrapperPass>());
    
    // get information out of passes
    LoopInfo& LI = LIPass.getLoopInfo();
    std::vector<const llvm::Loop*>& FunctionLoopList = DecLoopsPass.getLoopList();
    std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecLoopsInfo = DecLoopsPass.getDecoupleLoopsInfo_loop();
    if (FunctionLoopList.size() != DecLoopsInfo.size()) {
        llvm_unreachable("llvm::LoopInfo and ppar::DecoupleLoopInfo have a different number of loops!");
        return false;
    }

    // compute the metric for every single loop in the function
    for (const Loop* L : FunctionLoopList) {
        const auto iter = DecLoopsInfo.find(L);
        if (iter == DecLoopsInfo.end()) {
            llvm_unreachable("ppar::DecoupleLoopInfo doesn't have any information about the searched loop!");
            return false;
        }
        ppar::LoopDecoupleInfo* LI = (iter->second).get();
        Graph<llvm::Instruction*,ppar::Dependence*>& LoopPDG = PDGPass.getLoopGraph(L);

        auto& Payload = LI->getPayload();
        uint64_t LoopProperSCCsNumber_value = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            if ((*it)->getNodesNumber() > 1) {
                LoopProperSCCsNumber_value++;
            }
        }
   
        ValuePerLoop_loop[&F].insert(std::pair<const llvm::Loop*,double>(L, LoopProperSCCsNumber_value));
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_SCCS_NUMBER_H

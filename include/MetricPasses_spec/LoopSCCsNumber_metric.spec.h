#ifndef PPAR_LOOP_SCCS_NUMBER_H
#define PPAR_LOOP_SCCS_NUMBER_H

namespace ppar {

template<>
char MetricPass<ppar::LoopSCCsNumber>::ID = 0;

template <>
void MetricPass<ppar::LoopSCCsNumber>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
}

template <>
bool MetricPass<ppar::LoopSCCsNumber>::runOnFunction(Function& F) {
    const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecInfo_func 
        = (Pass::getAnalysis<DecoupleLoopsPass>()).getDecoupleLoopsInfo_func(); 
    
    for (auto iter = DecInfo_func.begin(); iter != DecInfo_func.end(); iter++) {
        const llvm::Loop* L = iter->first;
        const ppar::LoopDecoupleInfo* LI = (iter->second).get();
      
        // compute number of SCCs in the Payload of every loop
        const auto& Payload = LI->getPayload();
        uint64_t LoopSCCsNum = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            if ((*it)->getNodesNumber() > 1) {
                LoopSCCsNum++;
            }
        }

        // output computed metric
        ValuePerLoop_func[&F].insert(std::pair<const llvm::Loop*,double>(L, LoopSCCsNum));
    }

    const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecInfo_loop
        = (Pass::getAnalysis<DecoupleLoopsPass>()).getDecoupleLoopsInfo_loop(); 

    for (auto iter = DecInfo_loop.begin(); iter != DecInfo_loop.end(); iter++) {
        const llvm::Loop* L = iter->first;
        const ppar::LoopDecoupleInfo* LI = (iter->second).get();
      
        // compute number of SCCs in the Payload of every loop
        const auto& Payload = LI->getPayload();
        uint64_t LoopSCCsNum = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            if ((*it)->getNodesNumber() > 1) {
                LoopSCCsNum++;
            }
        }

        // output computed metric
        ValuePerLoop_loop[&F].insert(std::pair<const llvm::Loop*,double>(L, LoopSCCsNum));
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_SCCS_NUMBER_H

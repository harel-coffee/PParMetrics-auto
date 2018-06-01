#ifndef PPAR_LOOP_PAYLOAD_FRACTION_H
#define PPAR_LOOP_PAYLOAD_FRACTION_H

namespace ppar {

template<>
char MetricPass<ppar::LoopPayloadFraction>::ID = 0;

template <>
void MetricPass<ppar::LoopPayloadFraction>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopPayloadFraction>::runOnFunction(Function& F) {
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

    for (auto it = DecLoopsInfo.begin(); it != DecLoopsInfo.end(); it++) {
        const llvm::Loop* L = it->first;
        LoopDecoupleInfo* LI = (it->second).get();
      
        auto& Iterator = LI->getIterator();
        auto& Payload = LI->getPayload();
        uint64_t IteratorSize = 0;
        for (auto iter = Iterator.cbegin(); iter != Iterator.cend(); ++iter) {
            IteratorSize += (*iter)->getNodesNumber();
        }
        uint64_t PayloadSize = 0;
        for (auto iter = Payload.cbegin(); iter != Payload.cend(); ++iter) {
            PayloadSize += (*iter)->getNodesNumber();
        }

        double LoopPayloadFraction_value = static_cast<double>(IteratorSize)/(static_cast<double>(PayloadSize)+static_cast<double>(IteratorSize));

        ValuePerLoop_loop[&F].insert(std::pair<const llvm::Loop*,double>(L, LoopPayloadFraction_value));
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_PAYLOAD_FRACTION_H

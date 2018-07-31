#ifndef PPAR_LOOP_PROPORTION_METRICS_H
#define PPAR_LOOP_PROPORTION_METRICS_H

namespace ppar {

template<>
char MetricPass<ppar::LoopProportionMetrics>::ID = 0;

template <>
void MetricPass<ppar::LoopProportionMetrics>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopProportionMetrics>::runOnFunction(Function& F) {
    // get access to information passes, metric passes depend on
    GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>& PDGPass =
        Pass::getAnalysis<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
    DecoupleLoopsPass& DecLoopsPass = Pass::getAnalysis<DecoupleLoopsPass>(); 
    LoopInfoWrapperPass& LIPass = (getAnalysis<LoopInfoWrapperPass>());
    ppar::FunctionLoopInfoPass& LInfoPass = (getAnalysis<FunctionLoopInfoPass>());
    
    // get information out of passes
    LoopInfo& LI = LIPass.getLoopInfo();
    const FunctionLoopInfoPass::FunctionLoopList& LList = *LInfoPass.getFunctionLoopList(&F);
    std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecLoopsInfo = DecLoopsPass.getDecoupleLoopsInfo_loop();
    if (LList.size() != DecLoopsInfo.size()) {
        llvm_unreachable("llvm::LoopInfo and ppar::DecoupleLoopInfo have a different number of loops!");
        return false;
    }
        
    FunctionMetrics[&F] = std::make_unique<MetricSet_func<LoopProportionMetrics>>(&F);
    MetricSet_func<LoopProportionMetrics>* Metrics_func = (FunctionMetrics[&F]).get();

    // compute all metrics for every single loop in the function
    uint64_t idx;
    for (const Loop* L : LList) {
        auto it = DecLoopsInfo.find(L);
        if (it == DecLoopsInfo.end()) {
            llvm_unreachable("ppar::DecoupleLoopInfo doesn't have any information about the searched loop!");
            return false;
        }
        ppar::LoopDecoupleInfo* LI = (it->second).get();
        Graph<llvm::Instruction*,ppar::Dependence*>& LoopPDG = PDGPass.getLoopGraph(L);
        auto& Iterator = LI->getIterator();
        auto& Payload = LI->getPayload();

        Metrics_func->LoopMetrics[L] = std::make_unique<MetricSet_loop<LoopProportionMetrics>>(L); 
        MetricSet_loop<LoopProportionMetrics>* Metrics_loop = (Metrics_func->LoopMetrics[L]).get();
       
        // compute LOOP_ABSOLUTE_SIZE metric
        double LoopAbsoluteSize_value = LoopPDG.getNodesNumber();

        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_ABSOLUTE_SIZE;
        Metrics_loop->Metrics[idx] = LoopAbsoluteSize_value;

        // compute LOOP_PAYLOAD_FRACTION metric
        uint64_t IteratorSize = 0;
        for (auto iter = Iterator.cbegin(); iter != Iterator.cend(); ++iter) {
            IteratorSize += (*iter)->getNodesNumber();
        }
        uint64_t PayloadSize = 0;
        for (auto iter = Payload.cbegin(); iter != Payload.cend(); ++iter) {
            PayloadSize += (*iter)->getNodesNumber();
        }

        if ((IteratorSize + PayloadSize) != LoopPDG.getNodesNumber()) {
            llvm_unreachable("ppar::MetricPass<LoopProportionMetrics> : (IteratorSize + PayloadSize) != Loop PDG nodes number!");
            return false;
        }

        double LoopPayloadFraction_value = static_cast<double>(PayloadSize)/LoopAbsoluteSize_value;

        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PAYLOAD_FRACTION;
        Metrics_loop->Metrics[idx] = LoopPayloadFraction_value;

        // compute LOOP_PROPER_SCCS_NUMBER metric
        double LoopProperSCCsNumber_value = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            if ((*it)->getNodesNumber() > 1) {
                LoopProperSCCsNumber_value++;
            }
        }
 
        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PROPER_SCCS_NUMBER;
        Metrics_loop->Metrics[idx] = LoopProperSCCsNumber_value;

        // compute LOOP_CRITICAL_PAYLOAD_FRACTION metric
        double LoopCriticalPayloadFraction_value = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            if ((*it)->getNodesNumber() > 1) {
                LoopCriticalPayloadFraction_value += (*it)->getNodesNumber();
            }
        }
        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_CRITICAL_PAYLOAD_FRACTION;
        Metrics_loop->Metrics[idx] = LoopCriticalPayloadFraction_value/PayloadSize;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_PROPORTION_METRICS_H

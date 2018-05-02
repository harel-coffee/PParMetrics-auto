#ifndef PPAR_ITERATOR_PAYLOAD_RATIO_METRIC_H
#define PPAR_ITERATOR_PAYLOAD_RATIO_METRIC_H

namespace ppar {

template<>
char MetricPass<ppar::IteratorPayloadRatioMetricPass>::ID = 0;

template <>
void MetricPass<ppar::IteratorPayloadRatioMetricPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
}

template <>
bool MetricPass<ppar::IteratorPayloadRatioMetricPass>::runOnFunction(Function& F) {
    const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDependenceInfo>>& LoopsDepInfo 
        = (Pass::getAnalysis<DecoupleLoopsPass>()).getLoopsDepInfo(); 
    
    for (auto iter = LoopsDepInfo.begin(); iter != LoopsDepInfo.end(); iter++) {
        const llvm::Loop* L = iter->first;
        const LoopDependenceInfo* LI = (iter->second).get();
        
        double IteratorPayloadRatio = static_cast<double>(LI->getIterator().size())/static_cast<double>(LI->getPayload().size());
        outs() << "[PPar metric] Iterator/Payload ratio: " << IteratorPayloadRatio << "\n";
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_ITERATOR_PAYLOAD_RATIO_METRIC_H

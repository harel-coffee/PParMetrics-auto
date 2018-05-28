#ifndef PPAR_ITERATOR_LOOP_PERCENTAGE_H
#define PPAR_ITERATOR_LOOP_PERCENTAGE_H

namespace ppar {

template<>
char MetricPass<ppar::IteratorLoopPercentage>::ID = 0;

template <>
void MetricPass<ppar::IteratorLoopPercentage>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<DecoupleLoopsPass>();
}

template <>
bool MetricPass<ppar::IteratorLoopPercentage>::runOnFunction(Function& F) {
     const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecInfo_func 
        = (Pass::getAnalysis<DecoupleLoopsPass>()).getDecoupleLoopsInfo_func(); 
  
    for (auto iter = DecInfo_func.begin(); iter != DecInfo_func.end(); iter++) {
        const llvm::Loop* L = iter->first;
        const LoopDecoupleInfo* LI = (iter->second).get();
      
        // compute Iterator/Payload ratio metric
        const auto& Iterator = LI->getIterator();
        const auto& Payload = LI->getPayload();
        uint64_t IteratorSize = 0;
        for (auto it = Iterator.cbegin(); it != Iterator.cend(); ++it) {
            IteratorSize += (*it)->getNodesNumber();
        }
        uint64_t PayloadSize = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            PayloadSize += (*it)->getNodesNumber();
        }

        // output computed metric
        double IterLoopPercentage = static_cast<double>(IteratorSize)/(static_cast<double>(PayloadSize)+static_cast<double>(IteratorSize));
        ValuePerLoop_func[&F].insert(std::pair<const llvm::Loop*,double>(L, IterLoopPercentage));
    }

    const std::unordered_map<const llvm::Loop*, std::unique_ptr<LoopDecoupleInfo>>& DecInfo_loop
        = (Pass::getAnalysis<DecoupleLoopsPass>()).getDecoupleLoopsInfo_loop(); 

    for (auto iter = DecInfo_loop.begin(); iter != DecInfo_loop.end(); iter++) {
        const llvm::Loop* L = iter->first;
        const LoopDecoupleInfo* LI = (iter->second).get();
      
        // compute Iterator/Payload ratio metric
        const auto& Iterator = LI->getIterator();
        const auto& Payload = LI->getPayload();
        uint64_t IteratorSize = 0;
        for (auto it = Iterator.cbegin(); it != Iterator.cend(); ++it) {
            IteratorSize += (*it)->getNodesNumber();
        }
        uint64_t PayloadSize = 0;
        for (auto it = Payload.cbegin(); it != Payload.cend(); ++it) {
            PayloadSize += (*it)->getNodesNumber();
        }

        // output computed metric
        double IterLoopPercentage = static_cast<double>(IteratorSize)/(static_cast<double>(PayloadSize)+static_cast<double>(IteratorSize));
        ValuePerLoop_loop[&F].insert(std::pair<const llvm::Loop*,double>(L, IterLoopPercentage));
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_ITERATOR_LOOP_PERCENTAGE_H

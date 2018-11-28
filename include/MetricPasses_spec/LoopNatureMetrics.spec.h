#ifndef PPAR_LOOP_NATURE_METRICS_H
#define PPAR_LOOP_NATURE_METRICS_H

namespace ppar {

template<>
char MetricPass<ppar::LoopNatureMetrics>::ID = 0;

template <>
void MetricPass<ppar::LoopNatureMetrics>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopNatureMetrics>::runOnFunction(Function& F) {
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
        
    FunctionMetrics[&F] = std::make_unique<MetricSet_func<LoopNatureMetrics>>(&F);
    MetricSet_func<LoopNatureMetrics>* Metrics_func = (FunctionMetrics[&F]).get();

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

        Metrics_func->LoopMetrics[L] = std::make_unique<MetricSet_loop<LoopNatureMetrics>>(L); 
        MetricSet_loop<LoopNatureMetrics>* Metrics_loop = (Metrics_func->LoopMetrics[L]).get();
 
        // compute iterator related loop instruction nature metrics
        uint64_t MemoryWriteNum = 0;
        uint64_t CallInstrsNum = 0;
        uint64_t TotalSize = 0;
        for (auto iter = Iterator.cbegin(); iter != Iterator.cend(); ++iter) {
            const Graph<llvm::Instruction*,ppar::Dependence*>* IteratorSccPDG = (*iter);
            TotalSize += IteratorSccPDG->getNodesNumber();

            for (auto node_it = IteratorSccPDG->nodes_cbegin(); node_it != IteratorSccPDG->nodes_cend(); node_it++) {
                const GraphNode<llvm::Instruction*,ppar::Dependence*>& Node = *node_it;
                const llvm::Instruction* Inst = Node.getNode();

                if (Inst->mayWriteToMemory()) {
                    MemoryWriteNum++;
                }

                if (dyn_cast<llvm::CallInst>(Inst)) {
                    CallInstrsNum++;
                }
            }
        }

        idx = ppar::LoopNatureMetrics::NatureMetric_t::ITERATOR_MEM_WRITE_COUNT;
        Metrics_loop->Metrics[idx] = MemoryWriteNum;

        double IteratorMemWriteFraction_value = static_cast<double>(MemoryWriteNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::ITERATOR_MEM_WRITE_FRACTION;
        Metrics_loop->Metrics[idx] = IteratorMemWriteFraction_value;

        idx = ppar::LoopNatureMetrics::NatureMetric_t::ITERATOR_CALL_COUNT;
        Metrics_loop->Metrics[idx] = CallInstrsNum;

        double IteratorCallFraction_value = static_cast<double>(CallInstrsNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::ITERATOR_CALL_FRACTION;
        Metrics_loop->Metrics[idx] = IteratorCallFraction_value;
      
        // compute payload related loop instruction nature metrics
        MemoryWriteNum = 0;
        CallInstrsNum = 0;
        TotalSize = 0;
        for (auto iter = Payload.cbegin(); iter != Payload.cend(); ++iter) {
            const Graph<llvm::Instruction*,ppar::Dependence*>* PayoadSccPDG = (*iter);
            TotalSize += PayoadSccPDG->getNodesNumber();

            for (auto node_it = PayoadSccPDG->nodes_cbegin(); node_it != PayoadSccPDG->nodes_cend(); node_it++) {
                const GraphNode<Instruction*,ppar::Dependence*>& Node = *node_it;
                const llvm::Instruction* Inst = Node.getNode();

                if (Inst->mayWriteToMemory()) {
                    MemoryWriteNum++;
                }

                if (dyn_cast<llvm::CallInst>(Inst)) {
                    CallInstrsNum++;
                }
            }
        }

        idx = ppar::LoopNatureMetrics::NatureMetric_t::PAYLOAD_MEM_WRITE_COUNT;
        Metrics_loop->Metrics[idx] = MemoryWriteNum;

        double PayloadMemWriteFraction_value = static_cast<double>(MemoryWriteNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::PAYLOAD_MEM_WRITE_FRACTION;
        Metrics_loop->Metrics[idx] = PayloadMemWriteFraction_value;

        idx = ppar::LoopNatureMetrics::NatureMetric_t::PAYLOAD_CALL_COUNT;
        Metrics_loop->Metrics[idx] = CallInstrsNum;

        double PayloadCallFraction_value = static_cast<double>(CallInstrsNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::PAYLOAD_CALL_FRACTION;
        Metrics_loop->Metrics[idx] = PayloadCallFraction_value;

        // compute critical payload related loop instruction nature metrics
        MemoryWriteNum = 0;
        CallInstrsNum = 0;
        TotalSize = 0;
        for (auto iter = Payload.cbegin(); iter != Payload.cend(); ++iter) {
            if ((*iter)->getNodesNumber() > 1) {
                const Graph<llvm::Instruction*,ppar::Dependence*>* PayoadSccPDG = (*iter);
                TotalSize += PayoadSccPDG->getNodesNumber();

                for (auto node_it = PayoadSccPDG->nodes_cbegin(); node_it != PayoadSccPDG->nodes_cend(); node_it++) {
                    const GraphNode<llvm::Instruction*,ppar::Dependence*>& Node = *node_it;
                    const Instruction* Inst = Node.getNode();

                    if (Inst->mayWriteToMemory()) {
                        MemoryWriteNum++;
                    }

                    if (dyn_cast<llvm::CallInst>(Inst)) {
                        CallInstrsNum++;
                    }
                }
            }
        }

        idx = ppar::LoopNatureMetrics::NatureMetric_t::CRITICAL_PAYLOAD_MEM_WRITE_COUNT;
        Metrics_loop->Metrics[idx] = MemoryWriteNum;

        double CriticalPayloadMemWriteFraction_value = static_cast<double>(MemoryWriteNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::CRITICAL_PAYLOAD_MEM_WRITE_FRACTION;
        Metrics_loop->Metrics[idx] = CriticalPayloadMemWriteFraction_value;

        idx = ppar::LoopNatureMetrics::NatureMetric_t::CRITICAL_PAYLOAD_CALL_COUNT;
        Metrics_loop->Metrics[idx] = CallInstrsNum;

        double CriticalPayloadCallFraction_value = static_cast<double>(CallInstrsNum)/static_cast<double>(TotalSize);
        idx = ppar::LoopNatureMetrics::NatureMetric_t::CRITICAL_PAYLOAD_CALL_FRACTION;
        Metrics_loop->Metrics[idx] = CriticalPayloadCallFraction_value;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_NATURE_METRICS_H

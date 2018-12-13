#ifndef PPAR_LOOP_DEPENDENCE_METRICS_H
#define PPAR_LOOP_DEPENDENCE_METRICS_H

namespace ppar {

template<>
char MetricPass<ppar::LoopDependenceMetrics>::ID = 0;

template <>
void MetricPass<ppar::LoopDependenceMetrics>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<FunctionLoopInfoPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopDependenceMetrics>::runOnFunction(Function& F) {
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
        
    FunctionMetrics[&F] = std::make_unique<MetricSet_func<LoopDependenceMetrics>>(&F);
    MetricSet_func<LoopDependenceMetrics>* Metrics_func = (FunctionMetrics[&F]).get();

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

        Metrics_func->LoopMetrics[L] = std::make_unique<MetricSet_loop<LoopDependenceMetrics>>(L); 
        MetricSet_loop<LoopDependenceMetrics>* Metrics_loop = (Metrics_func->LoopMetrics[L]).get();
 
        uint64_t PayloadTotalDependenciesNum = 0;
        uint64_t PayloadTrueDependenciesNum = 0;
        uint64_t PayloadAntiDependenciesNum = 0;
        uint64_t PayloadOutputDependenciesNum = 0;
        uint64_t PayloadCrossDependenciesNum = 0;
        uint64_t CriticalPayloadTotalDependenciesNum = 0;
        uint64_t CriticalPayloadTrueDependenciesNum = 0;
        uint64_t CriticalPayloadAntiDependenciesNum = 0;
        uint64_t CriticalPayloadOutputDependenciesNum = 0;
        uint64_t CriticalPayloadCrossDependenciesNum = 0;
        for (auto edge_it = LoopPDG.edges_begin(); edge_it != LoopPDG.edges_end(); edge_it++) {
            std::pair<const llvm::Instruction*, const llvm::Instruction*> FromToPair = edge_it->first;
            typename Graph<llvm::Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
            
            Graph<llvm::Instruction*,ppar::Dependence*>* FromSCC = LoopPDG.nodeToSCC(const_cast<llvm::Instruction*>(FromToPair.first));
            Graph<llvm::Instruction*,ppar::Dependence*>* ToSCC = LoopPDG.nodeToSCC(const_cast<llvm::Instruction*>(FromToPair.second));
            // we are interested only in dependencies within the payload of the loop
            if (LI->SCCBelongsToPayload(FromSCC) && LI->SCCBelongsToPayload(ToSCC)) {
                if ((FromSCC == ToSCC) && (FromSCC->getNodesNumber() > 1)) {
                    for (const GraphEdge<llvm::Instruction*,ppar::Dependence*>& Edge : EdgeSet) {
                        if ((Edge.getData())->isFlow()) {
                            CriticalPayloadTrueDependenciesNum++;
                            PayloadTrueDependenciesNum++;
                        } else if ((Edge.getData())->isAnti()) {
                            CriticalPayloadAntiDependenciesNum++;
                            PayloadAntiDependenciesNum++;
                        } else if ((Edge.getData())->isOutput()) {
                            CriticalPayloadOutputDependenciesNum++;
                            PayloadOutputDependenciesNum++;
                        }
                        
                        uint64_t dir = (Edge.getData())->getDirection();
                        if ( (dir != llvm::Dependence::DVEntry::EQ) && 
                             (dir != llvm::Dependence::DVEntry::NONE) && 
                             (dir != llvm::Dependence::DVEntry::ALL) ) {
                            CriticalPayloadCrossDependenciesNum++;
                            PayloadCrossDependenciesNum++;
                        }

                        CriticalPayloadTotalDependenciesNum++;
                        PayloadTotalDependenciesNum++;
                    }
                } else {
                     for (const GraphEdge<llvm::Instruction*,ppar::Dependence*>& Edge : EdgeSet) {
                        if ((Edge.getData())->isFlow()) {
                            PayloadTrueDependenciesNum++;
                        } else if ((Edge.getData())->isAnti()) {
                            PayloadAntiDependenciesNum++;
                        } else if ((Edge.getData())->isOutput()) {
                            PayloadOutputDependenciesNum++;
                        }

                        uint64_t dir = (Edge.getData())->getDirection();
                        if ( (dir != llvm::Dependence::DVEntry::EQ) && 
                             (dir != llvm::Dependence::DVEntry::NONE) && 
                             (dir != llvm::Dependence::DVEntry::ALL) ) {
                            PayloadCrossDependenciesNum++;
                        }

                        PayloadTotalDependenciesNum++;
                    }
                }
            }
        }
              
        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_TOTAL_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = PayloadTotalDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_TRUE_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = PayloadTrueDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_ANTI_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = PayloadAntiDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_OUTPUT_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = PayloadOutputDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_CROSS_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = PayloadCrossDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_TOTAL_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = CriticalPayloadTotalDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_TRUE_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = CriticalPayloadTrueDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_ANTI_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = CriticalPayloadAntiDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_OUTPUT_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = CriticalPayloadOutputDependenciesNum;

        idx = ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_CROSS_DEPENDENCIES_NUM;
        Metrics_loop->Metrics[idx] = CriticalPayloadCrossDependenciesNum;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_PROPORTION_METRICS_H

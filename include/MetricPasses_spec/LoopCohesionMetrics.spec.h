#ifndef PPAR_LOOP_COHESION_METRICS_H
#define PPAR_LOOP_COHESION_METRICS_H

namespace ppar {

template<>
char MetricPass<ppar::LoopCohesionMetrics>::ID = 0;

template <>
void MetricPass<ppar::LoopCohesionMetrics>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::LoopCohesionMetrics>::runOnFunction(Function& F) {
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
    FunctionMetrics[&F] = std::make_unique<MetricSet_func<LoopCohesionMetrics>>(&F);
    MetricSet_func<LoopCohesionMetrics>* Metrics_func = (FunctionMetrics[&F]).get();

    // compute cohesion metrics for every single loop in the function
    uint64_t idx;
    for (const Loop* L : FunctionLoopList) {
        auto it = DecLoopsInfo.find(L);
        if (it == DecLoopsInfo.end()) {
            llvm_unreachable("ppar::DecoupleLoopInfo doesn't have any information about the searched loop!");
            return false;
        }
        ppar::LoopDecoupleInfo* LI = (it->second).get();
        Graph<llvm::Instruction*,ppar::Dependence*>& LoopPDG = PDGPass.getLoopGraph(L);
        auto& Iterator = LI->getIterator();
        auto& Payload = LI->getPayload();

        Metrics_func->LoopMetrics[L] = std::make_unique<MetricSet_loop<LoopCohesionMetrics>>(L); 
        MetricSet_loop<LoopCohesionMetrics>* Metrics_loop = (Metrics_func->LoopMetrics[L]).get();
       
        // compute Iterator / Payload cohesion metrics
        uint64_t LoopTotalEdgeCount = 0;
        uint64_t IteratorPayloadTotalEdgeCount = 0;
        uint64_t IteratorPayloadNonCFEdgeCount = 0;
        uint64_t IteratorPayloadMemEdgeCount = 0;
        for (auto edge_it = LoopPDG.edges_begin(); edge_it != LoopPDG.edges_end(); edge_it++) {
            std::pair<const llvm::Instruction*, const llvm::Instruction*> ToFromPair = edge_it->first;
            typename Graph<llvm::Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
           
            if ( (LI->nodeBelongsToIterator(ToFromPair.first) && LI->nodeBelongsToPayload(ToFromPair.second)) ||
                 (LI->nodeBelongsToPayload(ToFromPair.first) && LI->nodeBelongsToIterator(ToFromPair.second))) {
                
                for (const GraphEdge<llvm::Instruction*,ppar::Dependence*>& Edge : EdgeSet) {
                    if (!(Edge.getData())->isControl()) {
                        IteratorPayloadNonCFEdgeCount++;
                    }
                    if ((Edge.getData())->isMem()) {
                        IteratorPayloadMemEdgeCount++;
                    }
                }
                IteratorPayloadTotalEdgeCount += EdgeSet.size();
            }
            LoopTotalEdgeCount += EdgeSet.size();
        }

        double IteratorPayloadTotalCohesion_value = ((double)IteratorPayloadTotalEdgeCount)/((double)LoopTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_TOTAL_COHESION;
        Metrics_loop->Metrics[idx] = IteratorPayloadTotalCohesion_value;

        double IteratorPayloadNonCFCohesion_value = ((double)IteratorPayloadNonCFEdgeCount)/((double)LoopTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_NON_CF_COHESION;
        Metrics_loop->Metrics[idx] = IteratorPayloadNonCFCohesion_value;

        double IteratorPayloadMemCohesion_value = ((double)IteratorPayloadMemEdgeCount)/((double)LoopTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_MEM_COHESION;
        Metrics_loop->Metrics[idx] = IteratorPayloadMemCohesion_value;

        // compute Critical Payload / Payload cohesion metrics
        uint64_t LoopPayloadTotalEdgeCount = 0;
        uint64_t CriticalRegularPayloadTotalEdgeCount = 0;
        uint64_t CriticalRegularPayloadNonCFEdgeCount = 0;
        uint64_t CriticalRegularPayloadMemEdgeCount = 0;
        for (auto edge_it = LoopPDG.edges_begin(); edge_it != LoopPDG.edges_end(); edge_it++) {
            std::pair<const llvm::Instruction*, const llvm::Instruction*> ToFromPair = edge_it->first;
            typename Graph<llvm::Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
            
            Graph<llvm::Instruction*,ppar::Dependence*>* FromSCC = LoopPDG.nodeToSCC(const_cast<llvm::Instruction*>(ToFromPair.first));
            Graph<llvm::Instruction*,ppar::Dependence*>* ToSCC = LoopPDG.nodeToSCC(const_cast<llvm::Instruction*>(ToFromPair.second));
           
            if (LI->SCCBelongsToPayload(FromSCC) && LI->SCCBelongsToPayload(ToSCC)) {
                if ( ((FromSCC->getNodesNumber() > 1) && (ToSCC->getNodesNumber() == 1)) || 
                     ((ToSCC->getNodesNumber() > 1) && (FromSCC->getNodesNumber() == 1)) ) {
                    for (const GraphEdge<llvm::Instruction*,ppar::Dependence*>& Edge : EdgeSet) {
                        if (!(Edge.getData())->isControl()) {
                            CriticalRegularPayloadNonCFEdgeCount++;
                        }
                        if ((Edge.getData())->isMem()) {
                            CriticalRegularPayloadMemEdgeCount++;
                        }
                    }
                    CriticalRegularPayloadTotalEdgeCount += EdgeSet.size();
                }
                LoopPayloadTotalEdgeCount += EdgeSet.size();
            }
        }

        double CriticalPayloadTotalCohesion_value = ((double)CriticalRegularPayloadTotalEdgeCount)/((double)LoopPayloadTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_TOTAL_COHESION;
        Metrics_loop->Metrics[idx] = CriticalPayloadTotalCohesion_value;

        double CriticalPayloadNonCFCohesion_value = ((double)CriticalRegularPayloadNonCFEdgeCount)/((double)LoopPayloadTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_NON_CF_COHESION;
        Metrics_loop->Metrics[idx] = CriticalPayloadNonCFCohesion_value;

        double CriticalPayloadMemCohesion_value = ((double)CriticalRegularPayloadMemEdgeCount)/((double)LoopPayloadTotalEdgeCount);
        idx = ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_MEM_COHESION;
        Metrics_loop->Metrics[idx] = CriticalPayloadMemCohesion_value;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_COHESION_METRICS_H

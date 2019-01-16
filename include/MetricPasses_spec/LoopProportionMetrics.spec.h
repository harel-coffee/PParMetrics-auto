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
    AU.addRequired<GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>>();
    AU.addRequired<GraphPass<llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass>>();
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

        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_ITERATOR_SIZE;
        Metrics_loop->Metrics[idx] = IteratorSize;

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

        // compute LOOP_DEPTH metric
        double LoopDepth_value = L->getLoopDepth();

        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_DEPTH;
        Metrics_loop->Metrics[idx] = LoopDepth_value;

        // compute LOOP_INNER_LOOP_NUM metric
        double LoopInnerLoopNum_value = 0;
        std::queue<const llvm::Loop*> LQueue;
        std::vector<const llvm::Loop*> InnerLList;
        LQueue.push(L);
        while(!LQueue.empty()) {
            const llvm::Loop* CurrentLoop = LQueue.front();
            InnerLList.push_back(CurrentLoop);
            LQueue.pop();
            for (llvm::Loop::iterator inner_loop_it = CurrentLoop->begin(); inner_loop_it != CurrentLoop->end(); ++inner_loop_it) {
                LQueue.push(*inner_loop_it);
            }
        }
        LoopInnerLoopNum_value = InnerLList.size() - 1;

        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_INNER_LOOP_NUM;
        Metrics_loop->Metrics[idx] = LoopInnerLoopNum_value;

        // compute loop proportions on DDG and MDG graphs 

        const Graph<Instruction*,ppar::Dependence*>& ddgL =
            Pass::getAnalysis<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>>().getLoopGraph(L);
        if (ddgL == GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::InvalidGraph) {
            llvm_unreachable("llvm::Loop cannot have InvalidGraph allocated to it!");
        }

        if (!ddgL.isSCCsDataValid()) {
            ddgL.findSCCs();
        }
        
        if (!ddgL.isComponentGraphDataValid()) {
            ddgL.buildComponentGraph();
        }

        // compute LOOP_PROPER_SCCS_DDG_NUMBER metric
        double LoopProperSCCsDDGNumber_value = 0;
        for (Graph<Instruction*,ppar::Dependence*>::const_sccs_iterator scc_it = ddgL.sccs_cbegin(); scc_it != ddgL.sccs_cend(); scc_it++) {
            if ((scc_it->second)->getNodesNumber() > 1) {
                LoopProperSCCsDDGNumber_value++;
            }
        }
 
        idx = ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PROPER_SCCS_DDG_NUMBER;
        Metrics_loop->Metrics[idx] = LoopProperSCCsDDGNumber_value;
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_LOOP_PROPORTION_METRICS_H

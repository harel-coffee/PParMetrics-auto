#ifndef PPAR_ITERATOR_PAYLOAD_NON_CF_COHESION_H
#define PPAR_ITERATOR_PAYLOAD_NON_CF_COHESION_H

namespace ppar {

template<>
char MetricPass<ppar::IteratorPayloadNonCFCohesion>::ID = 0;

template <>
void MetricPass<ppar::IteratorPayloadNonCFCohesion>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DecoupleLoopsPass>();
    AU.addRequired<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
bool MetricPass<ppar::IteratorPayloadNonCFCohesion>::runOnFunction(Function& F) {
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
        auto it = DecLoopsInfo.find(L);
        if (it == DecLoopsInfo.end()) {
            llvm_unreachable("ppar::DecoupleLoopInfo doesn't have any information about the searched loop!");
            return false;
        }
        ppar::LoopDecoupleInfo* LI = (it->second).get();
        Graph<llvm::Instruction*,ppar::Dependence*>& LoopPDG = PDGPass.getLoopGraph(L);

        uint64_t LoopTotalEdgeCount = 0;
        uint64_t IteratorPayloadNonCFEdgeCount = 0;
        for (auto edge_it = LoopPDG.edges_begin(); edge_it != LoopPDG.edges_end(); edge_it++) {
            std::pair<const llvm::Instruction*, const llvm::Instruction*> ToFromPair = edge_it->first;
            typename Graph<llvm::Instruction*,ppar::Dependence*>::edge_set& EdgeSet = edge_it->second;
           
            uint64_t NonCFEdgesNum = 0;
            for (GraphEdge<llvm::Instruction*,ppar::Dependence*>& Edge : EdgeSet) {
                if (!(Edge.getData()).isControl()) {
                    NonCFEdgesNum++;
                }
            }

            if ( (LI->nodeBelongsToIterator(ToFromPair.first) && LI->nodeBelongsToPayload(ToFromPair.second)) ||
                 (LI->nodeBelongsToPayload(ToFromPair.first) && LI->nodeBelongsToIterator(ToFromPair.second))) {
                IteratorPayloadNonCFEdgeCount += NonCFEdgesNum;
            }
            LoopTotalEdgeCount += EdgeSet.size();
        }

        double IteratorPayloadNonCFCohesion_value = ((double)IteratorPayloadNonCFEdgeCount)/((double)LoopTotalEdgeCount)*100;

        ValuePerLoop_loop[&F].insert(std::pair<const llvm::Loop*,double>(L, IteratorPayloadTotalCohesion_value));
    }

    return false;
}

} // namespace ppar

#endif // #ifndef PPAR_ITERATOR_PAYLOAD_NON_CF_COHESION_H

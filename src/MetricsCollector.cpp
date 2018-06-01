#include "MetricsCollector.h"

#include "MetricPass.h"
#include "MetricPasses.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/LoopInfo.h"
using namespace llvm;

#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
using namespace std;

RegisterPass<ppar::MetricsCollector> MetricsCollectorPass(
    "ppar-metrics-collector",
    "Collect PPar metrics"
);

namespace ppar {

char MetricsCollector::ID = 0;

MetricsCollector::MetricsCollector() 
 : FunctionPass(ID) {}

void MetricsCollector::releaseMemory() {}

void MetricsCollector::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<ppar::MetricPass<ppar::LoopPayloadFraction>>();
    AU.addRequired<ppar::MetricPass<ppar::LoopProperSCCsNumber>>();
    AU.addRequired<ppar::MetricPass<ppar::IteratorPayloadTotalCohesion>>();
}

llvm::StringRef MetricsCollector::getPassName() const { 
    return "Metrics Collector Pass"; 
}

bool MetricsCollector::runOnFunction(llvm::Function& F) {
    // metrics file for function F 
    std::ofstream FuncMetricsFile(F.getName().str() + ".func.metrics");
    // function loops to print metrics for 
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    std::vector<const llvm::Loop*> FunctionLoops;
    std::queue<const llvm::Loop*> LoopsQueue;
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const llvm::Loop* TopLevelL = *loop_it;
        LoopsQueue.push(TopLevelL);
        while(!LoopsQueue.empty()) {
            const llvm::Loop* CurrentLoop = LoopsQueue.front();
            FunctionLoops.push_back(CurrentLoop);
            LoopsQueue.pop();
            for (auto sub_loop_it = CurrentLoop->begin(); sub_loop_it != CurrentLoop->end(); ++sub_loop_it) {
                LoopsQueue.push(*sub_loop_it);
            }
        }
    }

    // currently available metrics
    const std::unordered_map<const llvm::Loop*,double>* LoopPayloadFractionMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopPayloadFraction>>()).getMetricValues_loop(F); 

    const std::unordered_map<const llvm::Loop*,double>* LoopProperSCCsNumberMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopProperSCCsNumber>>()).getMetricValues_loop(F); 

    const std::unordered_map<const llvm::Loop*,double>* IterPayloadTotalCohesionMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::IteratorPayloadTotalCohesion>>()).getMetricValues_loop(F); 

    // fill function metrics file
    FuncMetricsFile << "[Pervasive Parallelism Metrics]\n\n";
    FuncMetricsFile << "Function: " << F.getName().str() << "\n";
  
    for (const Loop* L : FunctionLoops) {
        FuncMetricsFile << "Loop:\n";

        auto it = LoopPayloadFractionMetric->find(L);
        if (it != LoopPayloadFractionMetric->end()) {
            FuncMetricsFile << "\tloop-payload-fraction: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\tloop-payload-fraction:\n";
        }

        it = LoopProperSCCsNumberMetric->find(L);
        if (it != LoopProperSCCsNumberMetric->end()) {
            FuncMetricsFile << "\tloop-proper-sccs-num: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\tloop-proper-sccs-num:\n";
        }

        it = IterPayloadTotalCohesionMetric->find(L);
        if (it != IterPayloadTotalCohesionMetric->end()) {
            FuncMetricsFile << "\titer-payload-total-cohesion: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\titer-payload-total-cohesion:\n";
        }
    }

    FuncMetricsFile.close();

    return false;
}

} // namespace ppar

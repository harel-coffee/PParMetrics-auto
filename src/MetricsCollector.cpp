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
    AU.addRequired<ppar::MetricPass<ppar::IteratorLoopPercentage>>();
    AU.addRequired<ppar::MetricPass<ppar::LoopSCCsNumber>>();
}

llvm::StringRef MetricsCollector::getPassName() const { 
    return "Metrics Collector Pass"; 
}

bool MetricsCollector::runOnFunction(llvm::Function& F) {
    // metrics file for function F 
    std::ofstream FuncMetricsFile(F.getName().str() + ".func.metrics");
    // function loops to print metrics for 
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    std::vector<const Loop*> FunctionLoops;
    for (auto loop_it = LI.begin(); loop_it != LI.end(); ++loop_it) {
        const Loop* TopLevelL = *loop_it;
        FunctionLoops.push_back(TopLevelL);
        for (auto sub_loop_it = TopLevelL->begin(); sub_loop_it != TopLevelL->end(); ++sub_loop_it) {
            const Loop* SubL = *sub_loop_it;
            FunctionLoops.push_back(SubL);
        }
    }

    // currently available metrics
    const std::unordered_map<const llvm::Loop*,double>* IterLoopPercentageMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::IteratorLoopPercentage>>()).getMetricValues(F); 

    const std::unordered_map<const llvm::Loop*,double>* LoopSCCsNumberMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopSCCsNumber>>()).getMetricValues(F); 

    // fill function metrics file
    FuncMetricsFile << "[Pervasive Parallelism Metrics]\n\n";
    FuncMetricsFile << "Function: " << F.getName().str() << "\n";
   
    for (const Loop* L : FunctionLoops) {
        FuncMetricsFile << "Loop:\n";

        auto metric_it = IterLoopPercentageMetric->find(L);
        if (metric_it != IterLoopPercentageMetric->end()) {
            FuncMetricsFile << "\titer-percentage: " << metric_it->second << "\n";
        } else {
            FuncMetricsFile << "\titer-percentage:\n";
        }

        auto it = LoopSCCsNumberMetric->find(L);
        if (it != LoopSCCsNumberMetric->end()) {
            FuncMetricsFile << "\tloop-sccs-num: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\tloop-sccs-num:\n";
        }
    }

    FuncMetricsFile.close();

    return false;
}

} // namespace ppar

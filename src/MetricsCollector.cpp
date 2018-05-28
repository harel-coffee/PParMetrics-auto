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
    const std::unordered_map<const llvm::Loop*,double>* IterLoopPercentageMetric_func 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::IteratorLoopPercentage>>()).getMetricValues_func(F); 

    const std::unordered_map<const llvm::Loop*,double>* LoopSCCsNumberMetric_func 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopSCCsNumber>>()).getMetricValues_func(F); 

    const std::unordered_map<const llvm::Loop*,double>* IterLoopPercentageMetric_loop 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::IteratorLoopPercentage>>()).getMetricValues_loop(F); 

    const std::unordered_map<const llvm::Loop*,double>* LoopSCCsNumberMetric_loop
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopSCCsNumber>>()).getMetricValues_loop(F); 

    // fill function metrics file
    FuncMetricsFile << "[Pervasive Parallelism Metrics]\n\n";
    FuncMetricsFile << "Function: " << F.getName().str() << "\n";
  
    for (const Loop* L : FunctionLoops) {
        FuncMetricsFile << "Loop:\n";

        auto it = IterLoopPercentageMetric_func->find(L);
        if (it != IterLoopPercentageMetric_func->end()) {
            FuncMetricsFile << "\titer-percentage_func: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\titer-percentage_func:\n";
        }

        it = LoopSCCsNumberMetric_func->find(L);
        if (it != LoopSCCsNumberMetric_func->end()) {
            FuncMetricsFile << "\tloop-sccs-num_func: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\tloop-sccs-num_func:\n";
        }

        it = IterLoopPercentageMetric_loop->find(L);
        if (it != IterLoopPercentageMetric_loop->end()) {
            FuncMetricsFile << "\titer-percentage_loop: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\titer-percentage_loop:\n";
        }

        it = LoopSCCsNumberMetric_loop->find(L);
        if (it != LoopSCCsNumberMetric_loop->end()) {
            FuncMetricsFile << "\tloop-sccs-num_loop: " << it->second << "\n";
        } else {
            FuncMetricsFile << "\tloop-sccs-num_loop:\n";
        }
    }

    FuncMetricsFile.close();

    return false;
}

} // namespace ppar

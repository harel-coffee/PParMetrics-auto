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
}

llvm::StringRef MetricsCollector::getPassName() const { 
    return "Metrics Collector Pass"; 
}

bool MetricsCollector::runOnFunction(llvm::Function& F) {
    // metrics file for function F 
    std::ofstream FuncMetricsFile(F.getName().str() + ".func.metrics");
    // metrics files for every loop in the function F 
    const LoopInfo& LI = (getAnalysis<LoopInfoWrapperPass>()).getLoopInfo();
    std::unordered_map<const llvm::Loop*,std::ofstream*> LoopToFileStream;
    std::unordered_map<const llvm::Loop*,std::string> LoopAddrToName;
    int i = 0;
    for (Loop* L : LI) {
        LoopAddrToName[L] = std::string("loop") + std::to_string(i);
        i++;
        LoopToFileStream[L] = new std::ofstream(F.getName().str() + "." + LoopAddrToName[L] + ".loop.metrics");
    }

    const std::unordered_map<const llvm::Loop*,double>* IterLoopPercentageMetric 
        = (Pass::getAnalysis<ppar::MetricPass<ppar::IteratorLoopPercentage>>()).getMetricValues(F); 

    // fill function metrics file
    FuncMetricsFile << "[Pervasive Parallelism Metrics]\n";
    FuncMetricsFile << "Function: " << F.getName().str() << "\n\n";
   
    // fill loop metrics file
    for (Loop* L : LI) {
        auto metric_it = IterLoopPercentageMetric->find(L);
        if (metric_it != IterLoopPercentageMetric->end()) {
            FuncMetricsFile << LoopAddrToName[L] << ":\n";
            FuncMetricsFile << "\titer-percentage: " << metric_it->second << "\n";
            *LoopToFileStream[L] << LoopAddrToName[L] << ":\n";
            *LoopToFileStream[L] << "\titer-percentage: " << metric_it->second << "\n";
        } 

        for (auto inner_loop_it = L->begin(); inner_loop_it != L->end(); ++inner_loop_it) { 
            auto metric_it = IterLoopPercentageMetric->find(*inner_loop_it);
            if (metric_it != IterLoopPercentageMetric->end()) {
                FuncMetricsFile << LoopAddrToName[*inner_loop_it] << ":\n";
                FuncMetricsFile << "\titer-percentage: " << metric_it->second << "\n";
                *LoopToFileStream[*inner_loop_it] << LoopAddrToName[*inner_loop_it] << ":\n";
                *LoopToFileStream[*inner_loop_it] << "\titer-percentage: " << metric_it->second << "\n";
            } 
        }
    }

    for (Loop* L : LI) {
        LoopToFileStream[L]->close();
        delete LoopToFileStream[L];
    }
    LoopToFileStream.clear();
    FuncMetricsFile.close();

    return false;
}

} // namespace ppar


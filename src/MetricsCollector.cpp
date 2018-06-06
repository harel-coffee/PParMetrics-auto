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
#include "llvm/Support/FileSystem.h"
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
    AU.addRequired<ppar::MetricPass<ppar::LoopProportionMetrics>>();
    AU.addRequired<ppar::MetricPass<ppar::LoopCohesionMetrics>>();
}

llvm::StringRef MetricsCollector::getPassName() const { 
    return "Metrics Collector Pass"; 
}

bool MetricsCollector::runOnFunction(llvm::Function& F) {
    // metrics file for function F 
    std::error_code EC;
    llvm::raw_fd_ostream FuncMetricsFile(F.getName().str() + ".func.metrics", EC, sys::fs::F_Text);
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
    MetricSet_func<ppar::LoopProportionMetrics>* LoopProportionMetric_func
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopProportionMetrics>>()).getFunctionMetrics(F); 

    MetricSet_func<ppar::LoopCohesionMetrics>* LoopCohesionMetric_func
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopCohesionMetrics>>()).getFunctionMetrics(F); 

    // fill function metrics file
    FuncMetricsFile << "[ Pervasive Parallelism Metrics ]\n\n";
    FuncMetricsFile << "Function: " << F.getName().str() << " {\n\n";
 
    unsigned int loop_num = 1;
    for (const Loop* L : FunctionLoops) {
        FuncMetricsFile << "[" << loop_num++ << "] " << *L << "\n";        
        FuncMetricsFile << "Loop Proportion Metric Set:\n";
        if (LoopProportionMetric_func != nullptr) {
            MetricSet_loop<ppar::LoopProportionMetrics>* LoopProportionMetrics_loop = LoopProportionMetric_func->getLoopMetrics(L);
            if (LoopProportionMetrics_loop != nullptr) {
                double Metric = -1;
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_ABSOLUTE_SIZE);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-absolute-size: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tloop-absolute-size:\n";
                }
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PAYLOAD_FRACTION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-payload-fraction: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tloop-payload-fraction:\n";
                }
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PROPER_SCCS_NUMBER);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-proper-sccs-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tloop-proper-sccs-number:\n";
                }
           } else {
               FuncMetricsFile << "\tloop-absolute-size:\n";
               FuncMetricsFile << "\tloop-payload-fraction:\n";
               FuncMetricsFile << "\tloop-proper-sccs-number:\n";
           }
        } else {
            FuncMetricsFile << "\tNo Loop Proportion Metrics have been computed!\n";
        }
        FuncMetricsFile << "\n";

        FuncMetricsFile << "Loop Cohesion Metric Set:\n";

        if (LoopCohesionMetric_func != nullptr) {
            MetricSet_loop<ppar::LoopCohesionMetrics>* LoopCohesionMetrics_loop = LoopCohesionMetric_func->getLoopMetrics(L);
            if (LoopCohesionMetrics_loop != nullptr) {
                double Metric = -1;
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_TOTAL_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\titerator-payload-total-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\titerator-payload-total-cohesion:\n";
                }
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_NON_CF_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\titerator-payload-non-cf-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\titerator-payload-non-cf-cohesion:\n";
                }
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::ITERATOR_PAYLOAD_MEM_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\titerator-payload-mem-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\titerator-payload-mem-cohesion:\n";
                }
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_TOTAL_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-total-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-total-cohesion:\n";
                }
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_NON_CF_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-non-cf-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-non-cf-cohesion:\n";
                }
                Metric = LoopCohesionMetrics_loop->getMetricValue(ppar::LoopCohesionMetrics::CohesionMetric_t::CRITICAL_PAYLOAD_MEM_COHESION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-mem-cohesion: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-mem-cohesion:\n";
                }
            } else {
                FuncMetricsFile << "\titerator-payload-total-cohesion:\n";
                FuncMetricsFile << "\titerator-payload-non-cf-cohesion:\n";
                FuncMetricsFile << "\titerator-payload-mem-cohesion:\n";
                FuncMetricsFile << "\tcritical-payload-total-cohesion:\n";
                FuncMetricsFile << "\tcritical-payload-non-cf-cohesion:\n";
                FuncMetricsFile << "\tcritical-payload-mem-cohesion:\n";
            }
        } else {
            FuncMetricsFile << "\tNo Loop Cohesion Metrics have been computed!\n";
        }
        FuncMetricsFile << "\n";
    }
    FuncMetricsFile << "}\n";

    FuncMetricsFile.close();

    return false;
}

} // namespace ppar

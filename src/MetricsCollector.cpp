#include "MetricsCollector.h"

#include "MetricPass.h"
#include "MetricPasses.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Format.h"
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
    AU.addRequired<ppar::MetricPass<ppar::LoopDependenceMetrics>>();
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

    MetricSet_func<ppar::LoopDependenceMetrics>* LoopDependenceMetric_func
        = (Pass::getAnalysis<ppar::MetricPass<ppar::LoopDependenceMetrics>>()).getFunctionMetrics(F); 

    // fill function metrics file
    FuncMetricsFile << "[ Pervasive Parallelism Metrics ]\n\n";
    FuncMetricsFile << "Function: " << F.getName().str() << " {\n\n";
 
    unsigned int loop_num = 1;
    for (const Loop* L : FunctionLoops) {
        // print information identifying a loop in the function body
        FuncMetricsFile << "===== Loop [" << loop_num++ << "] =====\n";
        FuncMetricsFile << *L << "\n";
        uint64_t Line;
        StringRef File;
        StringRef Dir;
        FuncMetricsFile << "Loop can be found \n";
        for (typename Loop::block_iterator bb_it = L->block_begin(); bb_it != L->block_end(); ++bb_it) {
            for (typename BasicBlock::iterator inst_it = (*bb_it)->begin(); inst_it != (*bb_it)->end(); ++inst_it) {
                Instruction* I = &(*inst_it);
                if (DILocation *Loc = I->getDebugLoc()) {
                    if (I->isTerminator()) {
                        Line = Loc->getLine();
                        File = Loc->getFilename();
                        FuncMetricsFile << "at " << File << ":" << Line << "\n";
                    }
                }
            }
        }
        FuncMetricsFile << "\n";
        // print all computed metrics 
        FuncMetricsFile << "Loop Proportion Metric Set:\n";
        if (LoopProportionMetric_func != nullptr) {
            MetricSet_loop<ppar::LoopProportionMetrics>* LoopProportionMetrics_loop = LoopProportionMetric_func->getLoopMetrics(L);
            if (LoopProportionMetrics_loop != nullptr) {
                double Metric = -1;
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_ABSOLUTE_SIZE);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-absolute-size: " << llvm::format("%d", (uint64_t)Metric) << "\n";
                } else {
                    FuncMetricsFile << "\tloop-absolute-size:\n";
                }
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PAYLOAD_FRACTION);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-payload-fraction: " << llvm::format("%.4f", Metric) << "\n";
                } else {
                    FuncMetricsFile << "\tloop-payload-fraction:\n";
                    FuncMetricsFile << "\tloop-iterator-fraction:\n";
                }
                Metric = LoopProportionMetrics_loop->getMetricValue(ppar::LoopProportionMetrics::ProportionMetric_t::LOOP_PROPER_SCCS_NUMBER);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tloop-proper-sccs-number: " << llvm::format("%d", (uint64_t)Metric) << "\n";
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

        FuncMetricsFile << "Loop Dependence Metric Set:\n";

        if (LoopDependenceMetric_func != nullptr) {
            MetricSet_loop<ppar::LoopDependenceMetrics>* LoopDependenceMetrics_loop = LoopDependenceMetric_func->getLoopMetrics(L);
            if (LoopDependenceMetrics_loop != nullptr) {
                double Metric = -1;
                // print payload dependencies
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_TOTAL_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tpayload-total-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tpayload-total-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_TRUE_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tpayload-true-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tpayload-true-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_ANTI_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tpayload-anti-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tpayload-anti-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::PAYLOAD_OUTPUT_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tpayload-output-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tpayload-output-dependencies-number:\n";
                }
                // print critical payload dependencies
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_TOTAL_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-total-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-total-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_TRUE_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-true-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-true-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_ANTI_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-anti-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-anti-dependencies-number:\n";
                }
                Metric = LoopDependenceMetrics_loop->getMetricValue(ppar::LoopDependenceMetrics::DependenceMetric_t::CRITICAL_PAYLOAD_OUTPUT_DEPENDENCIES_NUM);
                if (Metric >= 0) {
                    FuncMetricsFile << "\tcritical-payload-output-dependencies-number: " << Metric << "\n";
                } else {
                    FuncMetricsFile << "\tcritical-payload-output-dependencies-number:\n";
                }
            } else {
                // print payload dependencies
                FuncMetricsFile << "\tpayload-total-dependencies-number:\n";
                FuncMetricsFile << "\tpayload-true-dependencies-number:\n";
                FuncMetricsFile << "\tpayload-anti-dependencies-number:\n";
                FuncMetricsFile << "\tpayload-output-dependencies-number:\n";
                // print critical payload dependencies
                FuncMetricsFile << "\tcritical-payload-total-dependencies-number:\n";
                FuncMetricsFile << "\tcritical-payload-true-dependencies-number:\n";
                FuncMetricsFile << "\tcritical-payload-anti-dependencies-number:\n";
                FuncMetricsFile << "\tcritical-payload-output-dependencies-number:\n";
            }
        } else {
            FuncMetricsFile << "\tNo Loop Dependence Metrics have been computed!\n";
        }
        FuncMetricsFile << "\n";
    }
    FuncMetricsFile << "}\n";

    FuncMetricsFile.close();

    return false;
}

} // namespace ppar

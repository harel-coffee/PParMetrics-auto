#ifndef PPAR_METRIC_PASS_H
#define PPAR_METRIC_PASS_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"

#include <unordered_map>

namespace ppar {
    
template <typename METRIC>
struct MetricPass;

template <typename METRIC>
struct MetricSet_loop {

    friend struct MetricPass<METRIC>;

    public:
        MetricSet_loop(const llvm::Loop* L = nullptr) {
            Loop_ptr = L;
            Metrics.resize(METRIC::MetricSubtype::METRIC_SUBTYPE_LAST);
        }

        ~MetricSet_loop() {
            Loop_ptr = nullptr;
            Metrics.clear();
        }
        
        double getMetricValue(uint64_t idx) { return Metrics[idx]; }

    private:
        std::vector<double> Metrics;
        const llvm::Loop* Loop_ptr;
};

template <typename METRIC>
struct MetricSet_func {
    
    friend class MetricPass<METRIC>;

    public:
        MetricSet_func(const llvm::Function* F = nullptr) {
            Function_ptr = F;
            LoopMetrics.clear();
        }

        ~MetricSet_func() {
            LoopMetrics.clear();
            Function_ptr = nullptr;
        }

        MetricSet_loop<METRIC>* getLoopMetrics(const llvm::Loop* L) { 
            auto it = LoopMetrics.find(L);
            if (it != LoopMetrics.end()) {
                return (it->second).get();
            } else {
                return nullptr;
            }
        }

    private:
        std::unordered_map<const llvm::Loop*, std::unique_ptr<MetricSet_loop<METRIC>>> LoopMetrics;
        const llvm::Function* Function_ptr;
};

template <typename METRIC>
struct MetricPass : public llvm::FunctionPass {
    public:
        static char ID;
        MetricPass();
        ~MetricPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

        MetricSet_func<METRIC>* getFunctionMetrics(const llvm::Function& F) { 
            auto it = FunctionMetrics.find(&F);
            if (it != FunctionMetrics.end()) {
                return (it->second).get();
            } else {
                return nullptr;
            }
        }

    private:
        METRIC MetricInfo;
        std::unordered_map<const llvm::Function*, std::unique_ptr<MetricSet_func<METRIC>>> FunctionMetrics;
};

} // namespace ppar

#endif // ifndef PPAR_METRIC_PASS_H

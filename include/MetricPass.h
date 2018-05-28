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
struct MetricPass : public llvm::FunctionPass {
    public:
        static char ID;
        MetricPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

        const std::unordered_map<const llvm::Loop*,double>* getMetricValues_loop(const llvm::Function& F) const { 
            auto it = ValuePerLoop_loop.find(&F);
            if (it != ValuePerLoop_loop.end()) {
                return &(it->second);
            } else {
                return nullptr;
            }
        }

        const std::unordered_map<const llvm::Loop*,double>* getMetricValues_func(const llvm::Function& F) const { 
            auto it = ValuePerLoop_func.find(&F);
            if (it != ValuePerLoop_func.end()) {
                return &(it->second);
            } else {
                return nullptr;
            }
        }

    private:
        METRIC MetricInfo;
        std::unordered_map<const llvm::Function*,std::unordered_map<const llvm::Loop*,double>> ValuePerLoop_func;
        std::unordered_map<const llvm::Function*,std::unordered_map<const llvm::Loop*,double>> ValuePerLoop_loop;
        std::unordered_map<const llvm::Function*,double> ValuePerFunc;
};

} // namespace ppar

#endif // ifndef PPAR_METRIC_PASS_H

#ifndef PPAR_METRIC_PASS_H
#define PPAR_METRIC_PASS_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

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

    private:
        METRIC MetricInfo;
};

} // namespace ppar

#endif // ifndef PPAR_METRIC_PASS_H

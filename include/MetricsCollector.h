#ifndef PPAR_METRICS_COLLECTOR_PASS_H
#define PPAR_METRICS_COLLECTOR_PASS_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

namespace ppar {

struct MetricsCollector : public llvm::FunctionPass {
    public:
        static char ID;
        MetricsCollector();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;
};

} // namespace ppar

#endif // ifndef PPAR_METRICS_COLLECTOR_PASS_H

#ifndef PPAR_METRIC_PRINTER_PASS_H
#define PPAR_METRIC_PRINTER_PASS_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"

#include <fstream>

namespace ppar {

struct MetricPrinter : public llvm::FunctionPass {
    public:
        static char ID;
        MetricPrinter();
        ~MetricPrinter();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

    private:
        std::ofstream MetricFileStream_excel;
        // format  
        const uint8_t FP_precision;
};

} // namespace ppar

#endif // ifndef PPAR_METRIC_PRINTER_PASS_H

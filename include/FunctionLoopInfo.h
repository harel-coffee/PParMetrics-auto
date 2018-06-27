#ifndef PPAR_FUNCTION_LOOP_INFO_H
#define PPAR_FUNCTION_LOOP_INFO_H

#include "ppar_common_includes.h"

#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"

#include <string>
#include <vector>
#include <unordered_map>

#undef DEBUG_TYPE
#define DEBUG_TYPE "func-loop-info"

namespace ppar {

struct FunctionLoopInfoPass : public llvm::FunctionPass {

    public:
        using FunctionLoopList = std::vector<const llvm::Loop*>;
        using LoopNames = std::unordered_map<const llvm::Loop*, std::string>;
        
        static char ID;
        FunctionLoopInfoPass();
        ~FunctionLoopInfoPass();

        bool runOnFunction(llvm::Function& F) override;
        void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
        llvm::StringRef getPassName() const;
        void releaseMemory() override;

        const FunctionLoopList* getFunctionLoopList(llvm::Function* F) { 
            auto it = FunctionLoopLists.find(F);
            if (it != FunctionLoopLists.end()) {
                return &(FunctionLoopLists[F]); 
            } else {
                return nullptr;
            }
        }
        
        const LoopNames* getFunctionLoopNames(llvm::Function* F) { 
            auto it = FunctionLoopNameMappings.find(F);
            if (it != FunctionLoopNameMappings.end()) {
                return &(FunctionLoopNameMappings[F]); 
            } else {
                return nullptr;
            }
        }

        std::string getFunctionLoopName(llvm::Function* F, llvm::Loop* L) {
            auto it = FunctionLoopNameMappings.find(F);
            if (it != FunctionLoopNameMappings.end()) {
                LoopNames& LNames = FunctionLoopNameMappings[F];
                auto it = LNames.find(L);
                if (it != LNames.end()) {
                    return LNames[L];
                } else {
                    return std::string("Undefined loop!");
                }
            } else {
                return std::string("Undefined function!");
            }
        }

    private:
        std::unordered_map<const llvm::Function*, FunctionLoopList> FunctionLoopLists;
        std::unordered_map<const llvm::Function*, LoopNames> FunctionLoopNameMappings;
};

} // namespace ppar

#endif // #ifndef PPAR_FUNCTION_LOOP_INFO_H

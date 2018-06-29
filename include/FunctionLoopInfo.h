/* -------------------- FunctionLoopInfo.h  -------------------- 
 * 
 * A wrapper pass around llvm::LoopInfo: puts all function loops 
 * into a linear list for further per-loop one-by-one processing.
 * 
 * Also it assigns a name to every single loop of the following 
 * format: function_name.depth_%N.loop_%N
 * %N is a consecutive integer number, depth starts from 1 (0 
 * is a scalar code outside of any loops), loop numbering starts 
 * from 0. It establishes consistent loop naming scheme across 
 * all passes of the tool.
 * 
 * -------------------------------------------------------------
 */

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

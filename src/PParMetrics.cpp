//===- PParMetrics.cpp - Code for Pervasive Parallelism metrics collection" -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//


#include "PParMetrics.h"

STATISTIC(PParCounter, "Counts PPar");

namespace 
{
    
bool PParMetrics::runOnFunction(Function& F) 
{
    DependenceInfo& DA = getAnalysis<DependenceAnalysisWrapperPass>().getDI();
    for (inst_iterator src_inst = inst_begin(&F); src_inst != inst_end(&F); ++src_inst) {
        if (isa<StoreInst>(*src_inst) || isa<LoadInst>(*src_inst)) {
            for (inst_iterator dst_inst = src_inst; dst_inst != inst_end(&F); ++dst_inst) {
                if (isa<StoreInst>(*dst_inst) || isa<LoadInst>(*dst_inst)) {
                    if ( std::unique_ptr<Dependence> dep = DA.depends(&*src_inst, &*dst_inst, true) ) 
                    {
                        PParCounter++;
                    }
                }
            }
        }
    }

    return false;
}

PParMetrics::PParMetrics() 
    : FunctionPass(ID) {}

void PParMetrics::getAnalysisUsage(AnalysisUsage &AU) const 
{
    AU.setPreservesAll();
    AU.addRequiredTransitive<DependenceAnalysisWrapperPass>();
}

char PParMetrics::ID = 0;

} // anonymous namespace

static RegisterPass<PParMetrics> pparMetrics("ppar-metrics", "computes software parallelisability metrics");

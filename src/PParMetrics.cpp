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
    return false;
}

PParMetrics::PParMetrics() 
    : FunctionPass(ID) {}

void PParMetrics::getAnalysisUsage(AnalysisUsage &AU) const 
{
    AU.setPreservesAll();
}

char PParMetrics::ID = 0;

} // anonymous namespace

static RegisterPass<PParMetrics> pparMetrics("ppar-metrics", "computes software parallelisability metrics");

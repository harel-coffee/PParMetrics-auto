//===-- llvm/Analysis/DependenceAnalysis.h -------------------- -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// DependenceAnalysis is an LLVM pass that analyses dependences between memory
// accesses. Currently, it is an implementation of the approach described in
//
//            Practical Dependence Testing
//            Goff, Kennedy, Tseng
//            PLDI 1991
//
// There's a single entry point that analyzes the dependence between a pair
// of memory references in a function, returning either NULL, for no dependence,
// or a more-or-less detailed description of the dependence between them.
//
// This pass exists to support the DependenceGraph pass. There are two separate
// passes because there's a useful separation of concerns. A dependence exists
// if two conditions are met:
//
//    1) Two instructions reference the same memory location, and
//    2) There is a flow of control leading from one instruction to the other.
//
// DependenceAnalysis attacks the first condition; DependenceGraph will attack
// the second (it's not yet ready).
//
// Please note that this is work in progress and the interface is subject to
// change.
//
// Plausible changes:
//    Return a set of more precise dependences instead of just one dependence
//    summarizing all.
//
//===----------------------------------------------------------------------===//

#ifndef PPAR_METRICS_ANALYSIS_H
#define PPAR_METRICS_ANALYSIS_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/DependenceAnalysis.h"
using namespace llvm;

#define DEBUG_TYPE "ppar-metrics"

#include "DependenceGraph.h"

namespace {
   
    class PParMetrics : public FunctionPass {
    public:
        static char ID; // Pass identification, replacement for typeid

        PParMetrics();
        bool runOnFunction(Function& F) override;

        void getAnalysisUsage(AnalysisUsage &AU) const override;
    };

} // anonymous namespace

#endif // #ifndef PPAR_METRICS_ANALYSIS_H

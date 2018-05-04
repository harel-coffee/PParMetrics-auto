#include "MetricPasses.h"
#include "DecoupleLoopsPass.h"

#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
using namespace llvm;

#include <string>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <utility>
#include <memory>
using namespace std;

#include "MetricPass_impl.h"

RegisterMetricPass(ppar::IteratorPayloadRatioMetricPass, IterWorkRatioMetricPass)
#include "IteratorPayloadRatio_metric.spec.h"

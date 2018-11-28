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

RegisterMetricPass(ppar::LoopProportionMetrics, LoopProportionMetricsPass)
#include "LoopProportionMetrics.spec.h"

RegisterMetricPass(ppar::LoopCohesionMetrics, LoopCohesionMetricsPass)
#include "LoopCohesionMetrics.spec.h"

RegisterMetricPass(ppar::LoopDependenceMetrics, LoopDependenceMetricsPass)
#include "LoopDependenceMetrics.spec.h"

RegisterMetricPass(ppar::LoopNatureMetrics, LoopNatureMetricsPass)
#include "LoopNatureMetrics.spec.h"

#include "GraphPasses.h"

#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/ScalarEvolution.h"
using namespace llvm;

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <utility>
#include <memory>
using namespace std;

#include "GraphPass_impl.h"

RegisterGraphPass(llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass,MDGraphPass)
#include "MemoryDependenceGraph_pass.spec.h"

RegisterGraphPass(llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass,DDGraphPass)
#include "DataDependenceGraph_pass.spec.h"

RegisterGraphPass(llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass,CDGraphPass)
#include "ControlDependenceGraph_pass.spec.h"

RegisterGraphPass(llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass,PDGraphPass)
#include "ProgramDependenceGraph_pass.spec.h"

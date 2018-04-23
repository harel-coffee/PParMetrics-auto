#include "GraphPrinter.h"
#include "GraphPass.h"
#include "ppar_common_includes.h"
#include "CustomGraphPasses.h"
#include "DotPrinter.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/DependenceAnalysis.h"
using namespace llvm;

#include <string>
#include <vector>
#include <memory>
using namespace std;

#include "GraphPrinter_impl.h"

RegisterGraphPrinterPass(llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass,MDGraphPrinterPass)
#include "MemoryDependenceGraph_printer.spec.h"

RegisterGraphPrinterPass(llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass,DDGraphPrinterPass)
#include "DataDependenceGraph_printer.spec.h"

RegisterGraphPrinterPass(llvm::BasicBlock*,ppar::Dependence*,ppar::ControlDependenceGraphPass,CDGraphPrinterPass)
#include "ControlDependenceGraph_printer.spec.h"

RegisterGraphPrinterPass(llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass,PDGraphPrinterPass)
#include "ProgramDependenceGraph_printer.spec.h"

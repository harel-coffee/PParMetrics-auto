#ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_SPEC_H
#define PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_SPEC_H

namespace ppar {

template <>
char GraphPrinter<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::ID = 0;

template <>
void GraphPrinter<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<ppar::GraphPass<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>>();
}

template <>
void GraphPrinter<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::buildDotNode(llvm::Instruction* Instr, DotNode* Node) {
    if (Instr->mayReadOrWriteMemory()) {
        // memory reference
        Node->setAttribute( /* name = */ string("shape"), /* value = */ string("ellipse"));
        Node->setAttribute( /* name = */ string("style"), /* value = */ string("filled"));
        Node->setAttribute( /* name = */ string("fillcolor"), /* value = */ string("lightcoral"));
    } else if (Instr->isTerminator()) {
        // jump instruction
        Node->setAttribute( /* name = */ string("shape"), /* value = */ string("ellipse"));
        Node->setAttribute( /* name = */ string("style"), /* value = */ string("filled"));
        Node->setAttribute( /* name = */ string("fillcolor"), /* value = */ string("lemonchiffon"));
    } else {
        Node->setAttribute( /* name = */ string("shape"), /* value = */ string("rectangle"));
        Node->setAttribute( /* name = */ string("style"), /* value = */ string("filled"));
        Node->setAttribute( /* name = */ string("fillcolor"), /* value = */ string("grey"));
    }

    string str;
    raw_string_ostream rso(str);
    Instr->print(rso);
    std::remove(str.begin(), str.end(), '\"');
    Node->setAttribute( /* name = */ string("label"), /* value = */ str);
}

template <>
void GraphPrinter<llvm::Instruction*,llvm::Dependence*,ppar::MemoryDependenceGraphPass>::buildDotEdge(llvm::Dependence* Dep, DotEdge* Edge) {}

} // namespace ppar

#endif // #ifndef PPAR_MEMORY_DEPENDENCE_GRAPH_PRINTER_SPEC_H

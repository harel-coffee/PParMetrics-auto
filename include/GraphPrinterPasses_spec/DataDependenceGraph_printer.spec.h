#ifndef PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_SPEC_H
#define PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_SPEC_H

namespace ppar {

template <>
char GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::ID = 0;

template <>
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::buildDotNode(const llvm::Instruction* Instr, DotNode* Node) {
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
    Node->setAttribute( /* name = */ string("label"), /* value = */ str);
}

template <>
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::DataDependenceGraphPass>::buildDotEdge(const ppar::Dependence* Dep, DotEdge* Edge) {
    // Flow + Reg data dependence
    Edge->setAttribute( /* name = */ string("label"), /* value = */ string("T"));
    Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("forestgreen"));
    Edge->setAttribute( /* name = */ string("color"), /* value = */ string("forestgreen"));
    Edge->setAttribute( /* name = */ string("style"), /* value = */ string("dotted"));
}

} // namespace ppar

#endif // #ifndef PPAR_DATA_DEPENDENCE_GRAPH_PRINTER_SPEC_H

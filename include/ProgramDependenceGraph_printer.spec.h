#ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H
#define PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H

namespace ppar {

template <>
char GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::ID = 0;

template <>
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AU.addRequiredTransitive<GraphPass<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>>();
}

template <>
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::buildDotNode(llvm::Instruction* Instr, DotNode* Node) {
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
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::buildDotEdge(ppar::Dependence* Dep, DotEdge* Edge) {
    if (Dep->isData()) {
        if (Dep->isFlow()) {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("T"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("forestgreen"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("forestgreen"));
        } else if (Dep->isAnti()) {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("A"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("firebrick3"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("firebrick3"));
        } else if (Dep->isOutput())   {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("O"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("dodgerblue"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("dodgerblue"));
        } else {
            Edge->setAttribute( /* name = */ string("label"), /* value = */ string("U"));
            Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("gray"));
            Edge->setAttribute( /* name = */ string("color"), /* value = */ string("gray"));
        }
        
        if (Dep->isMem()) {
            Edge->setAttribute( /* name = */ string("style"), /* value = */ string("solid"));
        } else {
            Edge->setAttribute( /* name = */ string("style"), /* value = */ string("dotted"));
        }
        
        if (Dep->isConfused()) {
            if (!Dep->isConsistent()) {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("1.0"));
            } else {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("0.0"));
            }
        } else {
            if (Dep->isConsistent()) {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("2.0"));
            } else {
                Edge->setAttribute( /* name = */ string("penwidth"), /* value = */ string("0.0"));
            }
        }
    } else if (Dep->isControl()) {
        Edge->setAttribute( /* name = */ string("label"), /* value = */ string("C"));
        Edge->setAttribute( /* name = */ string("fontcolor"), /* value = */ string("gold"));
        Edge->setAttribute( /* name = */ string("color"), /* value = */ string("gold"));
    }
}

} // namespace ppar

#endif // #ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H

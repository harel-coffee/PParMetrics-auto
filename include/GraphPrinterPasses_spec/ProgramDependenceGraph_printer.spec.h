#ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H
#define PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H

namespace ppar {

template <>
char GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::ID = 0;

template <>
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::buildDotNode(const llvm::Instruction* Instr, DotNode* Node) {
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
void GraphPrinter<llvm::Instruction*,ppar::Dependence*,ppar::ProgramDependenceGraphPass>::buildDotEdge(const ppar::Dependence* Dep, DotEdge* Edge) {
    std::string EdgeLabel {""};
    std::string DepType {"U"};
    std::string EdgeStyle {"solid"};
    std::string EdgeColor {"black"};
    std::string EdgePenWidth {"1.0"};
    std::string EdgeFontColor {"black"};
    const std::string DepConfused {"Conf"};
    const std::string DepConsistent {"Const"};
    const std::string DepDirections[] = {"0","<","=","<=",">","!=",">=","all"};

    uint64_t DepDir = 0;
    
    if (Dep->isData()) {
        if (Dep->isMem()) {
            if (Dep->isFlow()) {
                DepType = "T";
                EdgeColor = "forestgreen";
                EdgeFontColor = "forestgreen";
            } else if (Dep->isAnti()) {
                DepType = "A";
                EdgeColor = "firebrick3";
                EdgeFontColor = "firebrick3";
            } else if (Dep->isOutput())   {
                DepType = "O";
                EdgeColor = "dodgerblue";
                EdgeFontColor = "dodgerblue";
            } else {
                DepType = "U"; // unknown
                EdgeColor = "dodgerblue";
                EdgeFontColor = "dodgerblue";
            }
            EdgeLabel = DepType;

            if (Dep->isConfused()) {
                EdgeStyle = "dotted";
                EdgeLabel += ("|" + DepConfused);
            } else {
                if (Dep->isLoopIndependent()) {
                    EdgeLabel += ("|indep");
                } else {
                    EdgeLabel += ("|dep");
                }
                if (Dep->Distance != -1) {
                    EdgeLabel += ("|!");
                }
            }
 
            EdgeLabel += ("|" + DepDirections[Dep->getDirection()]);

            if (Dep->isConsistent()) {
                EdgeLabel += ("|" + DepConsistent);
            }
        } else {
            EdgeColor = "black";
            EdgeStyle = "solid";
            EdgePenWidth = "0.5";
            EdgeLabel = "";
        }
    } else if (Dep->isControl()) {
        EdgeColor = "gold";
        DepType = "C"; // control dependence
        EdgeFontColor = "gold";
        EdgeColor = "gold";
        EdgeLabel = DepType;
    }
    
    Edge->setAttribute( string("label"), EdgeLabel);
    Edge->setAttribute( string("fontcolor"), EdgeFontColor);
    Edge->setAttribute( string("color"), EdgeColor);
    Edge->setAttribute( string("style"), EdgeStyle);
    Edge->setAttribute( string("penwidth"), EdgePenWidth);
}

} // namespace ppar

#endif // #ifndef PPAR_PROGRAM_DEPENDENCE_GRAPH_PRINTER_SPEC_H

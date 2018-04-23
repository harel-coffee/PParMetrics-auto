#ifndef PPAR_CUSTOM_GRAPH_PASSES_H
#define PPAR_CUSTOM_GRAPH_PASSES_H

#include "ppar_common_includes.h"
#include <string>

namespace ppar {

struct MemoryDependenceGraphPass {
    public:
        MemoryDependenceGraphPass() {}

        static llvm::StringRef getPassName()
        { return "Memory Dependence Graph Pass"; } 
        
        static llvm::StringRef getGraphPassOption()
        { return "mdg"; }
        
        static llvm::StringRef getGraphPassOptionDesc()
        { return "Build in-memory Memory Dependence Graph (MDG) of the given function F"; }

        static llvm::StringRef getGraphPassDotPrintOption()
        { return "dot-mdg"; }
        
        static llvm::StringRef getGraphPassDotPrintOptionDesc()
        { return "Print Memory Dependence Graph (MDG) of the given function F into a DOT-formatted file"; }

        static std::string getDOTFileExtension()
        { return ".mdg.dot"; } 
};

struct DataDependenceGraphPass {
    public:
        DataDependenceGraphPass() {}

        static llvm::StringRef getPassName()
        { return "Data Dependence Graph Pass"; } 
        
        static llvm::StringRef getGraphPassOption()
        { return "ddg"; }
        
        static llvm::StringRef getGraphPassOptionDesc()
        { return "Build in-memory Data Dependence Graph (DDG) of the given function F"; }

        static llvm::StringRef getGraphPassDotPrintOption()
        { return "dot-ddg"; }
        
        static llvm::StringRef getGraphPassDotPrintOptionDesc()
        { return "Print Data Dependence Graph (DDG) of the given function F into a DOT-formatted file"; }

        static std::string getDOTFileExtension()
        { return ".ddg.dot"; } 
};

struct ControlDependenceGraphPass {
    public:
        ControlDependenceGraphPass() {}

        static llvm::StringRef getPassName()
        { return "Control Dependence Graph Pass"; } 
        
        static llvm::StringRef getGraphPassOption()
        { return "cdg"; }
        
        static llvm::StringRef getGraphPassOptionDesc()
        { return "Build in-memory Control Dependence Graph (CDG) of the given function F"; }

        static llvm::StringRef getGraphPassDotPrintOption()
        { return "dot-cdg"; }
        
        static llvm::StringRef getGraphPassDotPrintOptionDesc()
        { return "Print Control Dependence Graph (CDG) of the given function F into a DOT-formatted file"; }

        static std::string getDOTFileExtension()
        { return ".cdg.dot"; } 
};

struct ProgramDependenceGraphPass {
    public:
        ProgramDependenceGraphPass() {}

        static llvm::StringRef getPassName()
        { return "Program Dependence Graph Pass"; } 
        
        static llvm::StringRef getGraphPassOption()
        { return "pdg"; }
        
        static llvm::StringRef getGraphPassOptionDesc()
        { return "Build in-memory Program Dependence Graph (PDG) of the given function F"; }

        static llvm::StringRef getGraphPassDotPrintOption()
        { return "dot-pdg"; }
        
        static llvm::StringRef getGraphPassDotPrintOptionDesc()
        { return "Print Program Dependence Graph (PDG) of the given function F into a DOT-formatted file"; }

        static std::string getDOTFileExtension()
        { return ".pdg.dot"; } 
};

} // namespace ppar

#endif // #ifndef PPAR_CUSTOM_GRAPH_PASSES_H

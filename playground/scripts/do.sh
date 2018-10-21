#!/bin/bash

# 1) check for the necessary tools to use
ICC_COMPILER="/opt/intel/compilers_and_libraries/linux/bin/intel64/icc"
CLANG_COMPILER="/home/s1736883/Work/llvm/debug-build-60/bin/clang++"
GCC_COMPILER="/usr/bin/g++"
OPT_TOOL="/home/s1736883/Work/llvm/debug-build-60/bin/opt"
PPAR_METRICS_LIBRARY="/home/s1736883/Work/PParMetrics/build/libppar.so"

declare -a REQUIRED_TOOLS=("${ICC_COMPILER} 
                            ${GCC_COMPILER} 
                            ${CLANG_COMPILER} 
                            ${OPT_TOOL}
                            ${PPAR_METRICS_LIBRARY} ")

echo "1) Checking all the required tools"
for tool in ${GENERATED_OUTPUT_DIRS[@]}; do
    if [ ! -f ${tool} ]; then
        echo "Script failed: could not find ${tool} required for the run!"
        exit 1
    fi
    echo "${tool}: OK"
done

# 2) check for the existence of the main result directories
PLAYGROUND_DIR="${PWD}"
SOURCES_DIR="${PLAYGROUND_DIR}/sources"
ASSEMBLY_DIR="${PLAYGROUND_DIR}/assembly"
DOTS_DIR="${PLAYGROUND_DIR}/dot"
PDFS_DIR="${PLAYGROUND_DIR}/pdf"
EXECUTABLES_DIR="${PLAYGROUND_DIR}/exe"
METRICS_DIR="${PLAYGROUND_DIR}/metric"

declare -a NECESSARY_DIRS=("${ASSEMBLY_DIR} 
                            ${EXECUTABLES_DIR} 
                            ${DOTS_DIR} 
                            ${PDFS_DIR}
                            ${METRICS_DIR} ")

echo "2) Checking all the necessary directories"
for dir in ${NECESSARY_DIRS[@]}; do
    if [[ ! -e ${dir} ]]; then
        mkdir ${dir}
    fi
    echo "${dir}: OK"
done

COMPILER_FLAGS=""
COMPILER_FLAGS="${COMPILER_FLAGS} "

GCC_COMPILER_FLAGS="${COMPILER_FLAGS}"
ICC_COMPILER_FLAGS="${COMPILER_FLAGS}"
CLANG_COMPILER_FLAGS="${COMPILER_FLAGS} -emit-llvm -O0"

declare -a COMPILERS=( "${ICC_COMPILER}" "${GCC_COMPILER}" "${CLANG_COMPILER}" )
declare -a COMPILER_EXTS=( "icc" "gcc" "clang" )
declare -a COMPILER_FLAGS=( "${ICC_COMPILER_FLAGS}" "${GCC_COMPILER_FLAGS}" "${CLANG_COMPILER_FLAGS}" )

for src in $@; do
    echo "3) Checking all the necessary directories for sources: ${@}"
    for dir in ${NECESSARY_DIRS[@]}; do
        if [[ -d "${dir}/${src}" ]]; then
            rm -rf "${dir}/${src}"
        fi
        mkdir "${dir}/${src}"
        if [ ${dir} = ${DOTS_DIR} ] || [ ${dir} = ${PDFS_DIR} ]; then
            for GraphType in ddg mdg cdg pdg; do
                mkdir "${dir}/${src}/${GraphType}"
            done
        fi
    done

    # produce executables with different compilers
    for (( i=0; i < ${#COMPILERS[@]}; ++i )); do
        ${COMPILERS[i]} ${COMPILER_FLAGS[i]} "${SOURCES_DIR}/${src}.cpp" -o "${EXECUTABLES_DIR}/${src}/${src}.exe.${COMPILER_EXTS[i]}"
        ${COMPILERS[i]} ${COMPILER_FLAGS[i]} "-g" "${SOURCES_DIR}/${src}.cpp" -o "${EXECUTABLES_DIR}/${src}/${src}.g.exe.${COMPILER_EXTS[i]}"
    done

    # produce assembly files with different compilers
    for (( i=0; i < ${#COMPILERS[@]}; ++i )); do
        ${COMPILERS[i]} ${COMPILER_FLAGS[i]} "-S" "${SOURCES_DIR}/${src}.cpp" -o "${ASSEMBLY_DIR}/${src}/${src}.s.${COMPILER_EXTS[i]}"
        ${COMPILERS[i]} ${COMPILER_FLAGS[i]} "-S" "-g" "${SOURCES_DIR}/${src}.cpp" -o "${ASSEMBLY_DIR}/${src}/${src}.g.s.${COMPILER_EXTS[i]}"
    done

    # run PPar tool in order to collect PDGs of the source
    ASSEMBLY_FILE="${ASSEMBLY_DIR}/${src}/${src}.g.s.clang"

    for GraphType in ddg mdg cdg pdg; do
        echo "PPar metrics tool: generating ${GraphType} graphs"
        ${OPT_TOOL} "-load ${PPAR_METRICS_LIBRARY} -dot-${GraphType} ${ASSEMBLY_FILE}"
        mv *.dot "${DOTS_DIR}/${src}/${GraphType}"
    done

    for GraphType in ddg mdg cdg pdg; do
        echo "DOT->PDF files generation for ${GraphType} graphs"
        for DotFile in ${DOTS_DIR}/${src}/${GraphType}/*
        do
            echo $DotFile
            dot "-Tpdf ${DotFile} -o ${PDFS_DIR}/${src}/${GraphType}/${DotFile}.pdf"
        done
    done

    ${OPT_TOOL} "-load ${PPAR_METRICS_LIBRARY} -ppar-metrics-collector ${ASSEMBLY_FILE}"
    mv ./*.metrics ${METRICS_DIR}/${src}
done

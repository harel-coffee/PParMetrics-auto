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
REPORTS_DIR="${PLAYGROUND_DIR}/report"

declare -a NECESSARY_DIRS=("${ASSEMBLY_DIR} 
                            ${EXECUTABLES_DIR} 
                            ${DOTS_DIR} 
                            ${PDFS_DIR}
                            ${REPORTS_DIR}
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


OPT_OPTIONS=""
OPT_OPTIONS="${OPT_OPTIONS} -O0 -scev-aa"

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

    # produce ICC compiler optimization report
    ICC_PAR_REPORT_FLAGS=""
    ICC_PAR_REPORT_FLAGS="${ICC_PAR_REPORT_FLAGS} \
        -g \
        -xHost \
        -qopt-report-file=${REPORTS_DIR}/${src}/icc.report \
        -qopt-report-phase=vec,par,loop,openmp,ipo \
        -vec-threshold0 \
        -par-threshold0 \
        -unroll0 \
        -vec \
        -parallel \
        -no-ip \
        -fno-inline \
        -O2"
   
    ${ICC_COMPILER} ${ICC_PAR_REPORT_FLAGS} ${SOURCES_DIR}/${src}.cpp -o ${PLAYGROUND_DIR}/${src}.out
    rm -rf ${PLAYGROUND_DIR}/${src}.out

    # run PPar tool in order to collect PDGs of the source
    ASSEMBLY_FILE="${ASSEMBLY_DIR}/${src}/${src}.g.s.clang"

    for GraphType in ddg mdg cdg pdg; do
        echo "PPar metrics tool: generating ${GraphType} graphs"
        ${OPT_TOOL} ${OPT_OPTIONS} -load "${PPAR_METRICS_LIBRARY}" "-dot-${GraphType}" "${ASSEMBLY_FILE}"
        mv *.dot "${DOTS_DIR}/${src}/${GraphType}"
    done

    for GraphType in ddg mdg cdg pdg; do
        echo "DOT->PDF files generation for ${GraphType} graphs"
        for DotFile in ${DOTS_DIR}/${src}/${GraphType}/*
        do
            DotFile=${DotFile##*/}
            dot -Tpdf ${DOTS_DIR}/${src}/${GraphType}/${DotFile} -o ${PDFS_DIR}/${src}/${GraphType}/${DotFile}.pdf
        done
    done

    ${OPT_TOOL} ${OPT_OPTIONS} -load ${PPAR_METRICS_LIBRARY} -ppar-metrics-collector ${ASSEMBLY_FILE}
    mv ./*.metrics ${METRICS_DIR}/${src}
done

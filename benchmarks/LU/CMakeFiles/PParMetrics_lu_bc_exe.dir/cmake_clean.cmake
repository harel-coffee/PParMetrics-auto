file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_lu_link/PParMetrics_lu_link.bc"
  "llvm-ir/PParMetrics_lu_bc/blts.bc"
  "llvm-ir/PParMetrics_lu_bc/buts.bc"
  "llvm-ir/PParMetrics_lu_bc/domain.bc"
  "llvm-ir/PParMetrics_lu_bc/erhs.bc"
  "llvm-ir/PParMetrics_lu_bc/error.bc"
  "llvm-ir/PParMetrics_lu_bc/exact.bc"
  "llvm-ir/PParMetrics_lu_bc/jacld.bc"
  "llvm-ir/PParMetrics_lu_bc/jacu.bc"
  "llvm-ir/PParMetrics_lu_bc/l2norm.bc"
  "llvm-ir/PParMetrics_lu_bc/lu.bc"
  "llvm-ir/PParMetrics_lu_bc/pintgr.bc"
  "llvm-ir/PParMetrics_lu_bc/read_input.bc"
  "llvm-ir/PParMetrics_lu_bc/rhs.bc"
  "llvm-ir/PParMetrics_lu_bc/setbv.bc"
  "llvm-ir/PParMetrics_lu_bc/setcoeff.bc"
  "llvm-ir/PParMetrics_lu_bc/setiv.bc"
  "llvm-ir/PParMetrics_lu_bc/ssor.bc"
  "llvm-ir/PParMetrics_lu_bc/verify.bc"
  "llvm-ir/PParMetrics_lu_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_lu_bc/c_timers.bc"
  "llvm-ir/PParMetrics_lu_bc/print_results.bc"
  "llvm-ir/PParMetrics_lu_bc/wtime.bc"
  "llvm-ir/PParMetrics_lu_bc_exe/PParMetrics_lu_bc_exe.pdb"
  "llvm-ir/PParMetrics_lu_bc_exe/PParMetrics_lu_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_lu_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

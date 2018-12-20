file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_cg_link/PParMetrics_cg_link.bc"
  "llvm-ir/PParMetrics_cg_bc/cg.bc"
  "llvm-ir/PParMetrics_cg_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_cg_bc/c_timers.bc"
  "llvm-ir/PParMetrics_cg_bc/print_results.bc"
  "llvm-ir/PParMetrics_cg_bc/wtime.bc"
  "llvm-ir/PParMetrics_cg_bc/randdp.bc"
  "llvm-ir/PParMetrics_cg_bc_exe/PParMetrics_cg_bc_exe.pdb"
  "llvm-ir/PParMetrics_cg_bc_exe/PParMetrics_cg_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_cg_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

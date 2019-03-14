file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_mg_link/PParMetrics_mg_link.bc"
  "llvm-ir/PParMetrics_mg_bc/mg.bc"
  "llvm-ir/PParMetrics_mg_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_mg_bc/c_timers.bc"
  "llvm-ir/PParMetrics_mg_bc/print_results.bc"
  "llvm-ir/PParMetrics_mg_bc/wtime.bc"
  "llvm-ir/PParMetrics_mg_bc/randdp.bc"
  "llvm-ir/PParMetrics_mg_bc_exe/PParMetrics_mg_bc_exe.pdb"
  "llvm-ir/PParMetrics_mg_bc_exe/PParMetrics_mg_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_mg_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

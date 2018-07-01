file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_is_link/PParMetrics_is_link.bc"
  "llvm-ir/PParMetrics_is_bc/is.bc"
  "llvm-ir/PParMetrics_is_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_is_bc/c_timers.bc"
  "llvm-ir/PParMetrics_is_bc/print_results.bc"
  "llvm-ir/PParMetrics_is_bc/wtime.bc"
  "llvm-ir/PParMetrics_is_bc_exe/PParMetrics_is_bc_exe.pdb"
  "llvm-ir/PParMetrics_is_bc_exe/PParMetrics_is_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_is_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

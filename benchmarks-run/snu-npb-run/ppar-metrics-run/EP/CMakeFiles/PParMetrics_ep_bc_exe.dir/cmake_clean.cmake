file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_ep_link/PParMetrics_ep_link.bc"
  "llvm-ir/PParMetrics_ep_bc/ep.bc"
  "llvm-ir/PParMetrics_ep_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_ep_bc/c_timers.bc"
  "llvm-ir/PParMetrics_ep_bc/print_results.bc"
  "llvm-ir/PParMetrics_ep_bc/wtime.bc"
  "llvm-ir/PParMetrics_ep_bc/randdp.bc"
  "llvm-ir/PParMetrics_ep_bc_exe/PParMetrics_ep_bc_exe.pdb"
  "llvm-ir/PParMetrics_ep_bc_exe/PParMetrics_ep_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_ep_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

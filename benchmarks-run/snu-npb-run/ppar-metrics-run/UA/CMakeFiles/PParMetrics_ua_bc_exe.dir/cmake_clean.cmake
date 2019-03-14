file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_ua_link/PParMetrics_ua_link.bc"
  "llvm-ir/PParMetrics_ua_bc/adapt.bc"
  "llvm-ir/PParMetrics_ua_bc/convect.bc"
  "llvm-ir/PParMetrics_ua_bc/diffuse.bc"
  "llvm-ir/PParMetrics_ua_bc/mason.bc"
  "llvm-ir/PParMetrics_ua_bc/move.bc"
  "llvm-ir/PParMetrics_ua_bc/precond.bc"
  "llvm-ir/PParMetrics_ua_bc/setup.bc"
  "llvm-ir/PParMetrics_ua_bc/transfer.bc"
  "llvm-ir/PParMetrics_ua_bc/ua.bc"
  "llvm-ir/PParMetrics_ua_bc/utils.bc"
  "llvm-ir/PParMetrics_ua_bc/verify.bc"
  "llvm-ir/PParMetrics_ua_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_ua_bc/c_timers.bc"
  "llvm-ir/PParMetrics_ua_bc/print_results.bc"
  "llvm-ir/PParMetrics_ua_bc/wtime.bc"
  "llvm-ir/PParMetrics_ua_bc_exe/PParMetrics_ua_bc_exe.pdb"
  "llvm-ir/PParMetrics_ua_bc_exe/PParMetrics_ua_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_ua_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

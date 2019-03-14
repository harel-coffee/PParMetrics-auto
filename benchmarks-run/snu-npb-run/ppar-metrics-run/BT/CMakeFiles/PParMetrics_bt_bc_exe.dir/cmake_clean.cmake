file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_bt_link/PParMetrics_bt_link.bc"
  "llvm-ir/PParMetrics_bt_bc/add.bc"
  "llvm-ir/PParMetrics_bt_bc/adi.bc"
  "llvm-ir/PParMetrics_bt_bc/bt.bc"
  "llvm-ir/PParMetrics_bt_bc/error.bc"
  "llvm-ir/PParMetrics_bt_bc/exact_rhs.bc"
  "llvm-ir/PParMetrics_bt_bc/exact_solution.bc"
  "llvm-ir/PParMetrics_bt_bc/initialize.bc"
  "llvm-ir/PParMetrics_bt_bc/rhs.bc"
  "llvm-ir/PParMetrics_bt_bc/set_constants.bc"
  "llvm-ir/PParMetrics_bt_bc/solve_subs.bc"
  "llvm-ir/PParMetrics_bt_bc/verify.bc"
  "llvm-ir/PParMetrics_bt_bc/x_solve.bc"
  "llvm-ir/PParMetrics_bt_bc/y_solve.bc"
  "llvm-ir/PParMetrics_bt_bc/z_solve.bc"
  "llvm-ir/PParMetrics_bt_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_bt_bc/c_timers.bc"
  "llvm-ir/PParMetrics_bt_bc/print_results.bc"
  "llvm-ir/PParMetrics_bt_bc/wtime.bc"
  "llvm-ir/PParMetrics_bt_bc/randdp.bc"
  "llvm-ir/PParMetrics_bt_bc_exe/PParMetrics_bt_bc_exe.pdb"
  "llvm-ir/PParMetrics_bt_bc_exe/PParMetrics_bt_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_bt_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

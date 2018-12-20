file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/PParMetrics_ft_link/PParMetrics_ft_link.bc"
  "llvm-ir/PParMetrics_ft_bc/ft.bc"
  "llvm-ir/PParMetrics_ft_bc/c_print_results.bc"
  "llvm-ir/PParMetrics_ft_bc/c_timers.bc"
  "llvm-ir/PParMetrics_ft_bc/print_results.bc"
  "llvm-ir/PParMetrics_ft_bc/wtime.bc"
  "llvm-ir/PParMetrics_ft_bc/randdp.bc"
  "llvm-ir/PParMetrics_ft_bc_exe/PParMetrics_ft_bc_exe.pdb"
  "llvm-ir/PParMetrics_ft_bc_exe/PParMetrics_ft_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics_ft_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_ep_link/BasicBitcodeGen_ep_link.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/ep.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc/randdp.bc"
  "llvm-ir/BasicBitcodeGen_ep_bc_exe/BasicBitcodeGen_ep_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_ep_bc_exe/BasicBitcodeGen_ep_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_ep_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

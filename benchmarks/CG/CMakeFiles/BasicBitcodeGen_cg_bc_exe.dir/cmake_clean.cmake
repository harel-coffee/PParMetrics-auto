file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_cg_link/BasicBitcodeGen_cg_link.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/cg.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc/randdp.bc"
  "llvm-ir/BasicBitcodeGen_cg_bc_exe/BasicBitcodeGen_cg_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_cg_bc_exe/BasicBitcodeGen_cg_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_cg_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

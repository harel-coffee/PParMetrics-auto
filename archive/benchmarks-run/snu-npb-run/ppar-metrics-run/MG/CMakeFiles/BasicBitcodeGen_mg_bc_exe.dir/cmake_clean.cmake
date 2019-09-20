file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_mg_link/BasicBitcodeGen_mg_link.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/mg.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc/randdp.bc"
  "llvm-ir/BasicBitcodeGen_mg_bc_exe/BasicBitcodeGen_mg_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_mg_bc_exe/BasicBitcodeGen_mg_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_mg_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

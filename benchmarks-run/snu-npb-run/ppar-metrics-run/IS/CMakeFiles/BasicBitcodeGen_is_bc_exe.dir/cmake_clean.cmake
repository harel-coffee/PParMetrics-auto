file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_is_link/BasicBitcodeGen_is_link.bc"
  "llvm-ir/BasicBitcodeGen_is_bc/is.bc"
  "llvm-ir/BasicBitcodeGen_is_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_is_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_is_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_is_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_is_bc_exe/BasicBitcodeGen_is_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_is_bc_exe/BasicBitcodeGen_is_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_is_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

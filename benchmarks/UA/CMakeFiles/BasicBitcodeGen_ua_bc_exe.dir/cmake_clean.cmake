file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_ua_link/BasicBitcodeGen_ua_link.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/adapt.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/convect.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/diffuse.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/mason.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/move.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/precond.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/setup.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/transfer.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/ua.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/utils.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/verify.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_ua_bc_exe/BasicBitcodeGen_ua_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_ua_bc_exe/BasicBitcodeGen_ua_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_ua_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

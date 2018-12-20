file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_sp_link/BasicBitcodeGen_sp_link.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/add.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/adi.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/error.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/exact_rhs.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/exact_solution.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/initialize.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/ninvr.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/pinvr.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/rhs.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/set_constants.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/sp.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/txinvr.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/tzetar.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/verify.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/x_solve.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/y_solve.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/z_solve.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_sp_bc_exe/BasicBitcodeGen_sp_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_sp_bc_exe/BasicBitcodeGen_sp_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_sp_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

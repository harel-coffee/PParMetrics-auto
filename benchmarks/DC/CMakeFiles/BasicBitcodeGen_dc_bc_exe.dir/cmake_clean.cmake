file(REMOVE_RECURSE
  "npbparams.h"
  "llvm-ir/BasicBitcodeGen_dc_link/BasicBitcodeGen_dc_link.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/adc.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/dc.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/extbuild.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/jobcntl.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/rbt.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/c_print_results.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/c_timers.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/print_results.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/wtime.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc/randdp.bc"
  "llvm-ir/BasicBitcodeGen_dc_bc_exe/BasicBitcodeGen_dc_bc_exe.pdb"
  "llvm-ir/BasicBitcodeGen_dc_bc_exe/BasicBitcodeGen_dc_bc_exe"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/BasicBitcodeGen_dc_bc_exe.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

add_llvm_loadable_module( PParMetrics
  PParMetrics.cpp
  DependenceGraph.cpp
  DataDependenceGraph.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )

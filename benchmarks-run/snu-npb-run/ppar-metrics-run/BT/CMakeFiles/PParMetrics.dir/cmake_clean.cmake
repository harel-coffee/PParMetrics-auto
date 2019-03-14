file(REMOVE_RECURSE
  "npbparams.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/PParMetrics.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

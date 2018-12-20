
if(COMMAND BmkBasicInstallPipeline)
  BmkBasicInstallPipeline(${BMK_PROJECT_NAME})
else()
  message(WARNING "Could not execute command: BmkBasicInstallPipeline")
endif()
  
if(COMMAND BasicBitcodeGenPipeline)
  BasicBitcodeGenPipeline(${BMK_PROJECT_NAME})
else()
  message(WARNING "Could not execute command: BasicBitcodeGenPipeline")
endif()
  
if(COMMAND PParMetricsPipeline)
  PParMetricsPipeline(${BMK_PROJECT_NAME})
else()
  message(WARNING "Could not execute command: PParMetricsPipeline")
endif()
  
add_llvm_library(AOS
  AOSGlobalVariableOptPass.cpp
  AOSOptPass.cpp
  AOSMallocPass.cpp
  AOSBBCounterPass.cpp
  AOSPointerAliasPass.cpp
  AOSMarkTaintPass.cpp
  AOSReachTestPass.cpp
  AOS.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/AOS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Support
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/IR
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Analysis

  DEPENDS
  intrinsics_gen
  LLVMCore
  LLVMSupport
  LLVMTransformUtils
  LLVMCodeGen
  LLVMAnalysis
  PARTSsha3

  LINK_LIBS
  LLVMCore
  LLVMSupport
  LLVMTransformUtils
  LLVMCodeGen
  LLVMAnalysis
  PARTSsha3
  )

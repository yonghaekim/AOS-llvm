add_llvm_library(WYFY
  WyfyPtrAliasPass.cpp
  WyfyTaintPropPass.cpp
  WyfyDataPtrSignPass.cpp
  WyfyFuncCounterPass.cpp
	WYFY.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/WYFY
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

#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/WYFY/WYFY.h"
#include "llvm/Support/CommandLine.h"
#include <regex>
#include <iostream>

using namespace llvm;
using namespace WYFY;

//namespace {
  class WyfyFuncCounterPass : public ModulePass {

  public:
    static char ID; // Pass identification, replacement for typeid
    WyfyFuncCounterPass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override;

  private:  
		bool handleFunction(Function &F);
  };
//}


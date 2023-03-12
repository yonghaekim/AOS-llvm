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
#include "llvm/AOS/AOS.h"
#include <iostream>

using namespace llvm;
using namespace AOS;

#define DEBUG_TYPE "aos_opt_pass"

STATISTIC(StatSignDataPointer, "Number of data pointers signed");
STATISTIC(StatStripDataPointer, "Number of data pointers stripped");

namespace {
  class AOSOptPass : public BasicBlockPass {

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSOptPass() : BasicBlockPass(ID) {}

    bool runOnBasicBlock(BasicBlock &BB) override;

  private:  
    bool handleMalloc(Function *pF, CallInst *pCI);
    bool handleCalloc(Function *pF, CallInst *pCI);
    bool handleRealloc(Function *pF, CallInst *pCI);
    bool handleFree(Function *pF, CallInst *pCI);
  };
}

char AOSOptPass::ID = 0;
static RegisterPass<AOSOptPass> X("aos", "AOS opt pass");

Pass *llvm::AOS::createAOSOptPass() { return new AOSOptPass(); }

bool AOSOptPass::runOnBasicBlock(BasicBlock &BB) {
  bool basicblock_modified = false;

  for (auto &I : BB) {
    if (CallInst *CI = dyn_cast<CallInst>(&I)) {
      Function *pF = CI->getCalledFunction();

      if (pF && pF->getName() == "malloc") {
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "calloc") {
        basicblock_modified = handleCalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "realloc") {
        basicblock_modified = handleRealloc(pF, CI) || basicblock_modified;     
      } else if (pF && pF->getName() == "free") {
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_Znwm") { // new
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_Znam") { // new[]
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_ZdlPv") { //delete
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_ZdaPv") { // delete[]
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      }
    }
  }

  return basicblock_modified;
}

bool AOSOptPass::handleMalloc(Function *pF, CallInst *pCI) {
  auto arg = pCI->getArgOperand(0);

  std::vector<Type *> arg_type;
  arg_type.push_back(pCI->getType());

  IRBuilder<> Builder(pCI->getNextNode());
  auto malloc = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_malloc, arg_type);

  Builder.CreateCall(malloc, {pCI, arg}, "");

  StatSignDataPointer++;

  return true;
}

bool AOSOptPass::handleCalloc(Function *pF, CallInst *pCI) {
  auto arg0 = pCI->getArgOperand(0);
  auto arg1 = pCI->getArgOperand(1);

  std::vector<Type *> arg_type;
  arg_type.push_back(pCI->getType());

  IRBuilder<> Builder_prev(pCI);
  Value *res = Builder_prev.CreateMul(arg0, arg1);

  IRBuilder<> Builder(pCI->getNextNode());
  auto calloc = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_calloc, arg_type);

  Builder.CreateCall(calloc, {pCI, res}, "");

  StatSignDataPointer++;

  return true;
}

bool AOSOptPass::handleRealloc(Function *pF, CallInst *pCI) {
  auto arg = pCI->getArgOperand(1);

  std::vector<Type *> arg_type;
  arg_type.push_back(pCI->getType());

  IRBuilder<> Builder(pCI->getNextNode());
  auto realloc = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_realloc, arg_type);

  Builder.CreateCall(realloc, {pCI, arg}, "");

  StatSignDataPointer++;

  return true;
}

bool AOSOptPass::handleFree(Function *pF, CallInst *pCI) {
  auto arg = pCI->getArgOperand(0);

  std::vector<Type *> arg_type;
  arg_type.push_back(arg->getType());

  IRBuilder<> Builder(pCI->getNextNode());

  auto free = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_free, arg_type);

  Builder.CreateCall(free, {arg}, "");

  StatStripDataPointer++;

  return true;
}


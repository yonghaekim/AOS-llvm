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

using namespace llvm;
using namespace AOS;

#define DEBUG_TYPE "aos_gvar_opt_pass"

namespace {
  class AOSGlobalVariableOptPass : public ModulePass {

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSGlobalVariableOptPass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override;
  };
}

char AOSGlobalVariableOptPass::ID = 0;
static RegisterPass<AOSGlobalVariableOptPass> X("aos-global", "AOS global variable opt pass");

Pass *llvm::AOS::createAOSGlobalVariableOptPass() { return new AOSGlobalVariableOptPass(); }

bool AOSGlobalVariableOptPass::runOnModule(Module &M) {
  M.getOrInsertGlobal("metadata", ArrayType::get(Type::getInt64Ty(M.getContext()), 16384));

  GlobalVariable* GArray = M.getNamedGlobal("metadata");
  GArray->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
  GArray->setAlignment(8);

  return true;
}



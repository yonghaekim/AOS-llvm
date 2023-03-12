#include "llvm/WYFY/WyfyFuncCounterPass.h"


char WyfyFuncCounterPass::ID = 0;
static RegisterPass<WyfyFuncCounterPass> X("wyfy-fcnt", "WYFY function counter pass");

Pass *llvm::WYFY::createWyfyFuncCounterPass() { return new WyfyFuncCounterPass(); }

bool WyfyFuncCounterPass::runOnModule(Module &M) {
	bool function_modified = false;

	for (auto &F : M) {
		if (&F && !F.isDeclaration()) {
	    function_modified = handleFunction(F) || function_modified;
    }
  }

  return function_modified;
}

bool WyfyFuncCounterPass::handleFunction(Function &F) {
	auto &BBF = F.front();
  auto &IF = BBF.front();
	Instruction *pI = &IF;

	Module *pM = F.getParent();
  IRBuilder<> Builder(pI);

  auto fcnt = Intrinsic::getDeclaration(F.getParent(), Intrinsic::wyfy_fcnt);
  Builder.CreateCall(fcnt, {}, "");

	return true;
}


#include "llvm/AOS/AOSBBCounterPass.h"


char AOSBBCounterPass::ID = 0;
static RegisterPass<AOSBBCounterPass> X("aos-bbc", "AOS basic block counter pass");

Pass *llvm::AOS::createAOSBBCounterPass() { return new AOSBBCounterPass(); }

bool AOSBBCounterPass::runOnModule(Module &M) {
	bool function_modified = false;

	for (auto &F : M) {
		if (&F && !F.isDeclaration()) {
	    function_modified = handleFunction(F) || function_modified;
    }
  }

  return function_modified;
}

bool AOSBBCounterPass::handleFunction(Function &F) {
	auto &BBF = F.front();
  auto &IF = BBF.front();
	Instruction *pI = &IF;

	//while (true) {
	//	if (pI->getOpcode() == Instruction::PHI ||
	//			pI->getOpcode() == Instruction::LandingPad) {
	//		pI = pI->getNextNode();
	//	} else {
	//		break;
	//	}
	//}

	Module *pM = F.getParent();
  IRBuilder<> Builder(pI);

  auto fcnt = Intrinsic::getDeclaration(F.getParent(), Intrinsic::wyfy_fcnt);
  Builder.CreateCall(fcnt, {}, "");

	//Value *arg = ConstantInt::get(Type::getInt64Ty(pM->getContext()), func_num++);
	//FunctionType *FuncTypeA = FunctionType::get(Type::getVoidTy(pM->getContext()), {Type::getInt64Ty(pM->getContext())}, false);
	//Constant *promote = F.getParent()->getOrInsertFunction("wyfy_print_func", FuncTypeA);
	//auto callA = Builder.CreateCall(promote, {arg});

	return true;
}


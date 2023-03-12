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
#include "llvm/AOS/AOS.h"
#include <iostream>

using namespace llvm;
using namespace AOS;

#define DEBUG_TYPE "aos_input_pass"

namespace {
  class AOSInputDetectPass : public BasicBlockPass {

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSInputDetectPass() : BasicBlockPass(ID) {}

    bool runOnBasicBlock(BasicBlock &BB) override;

  private:  
    bool handleInputFunc(Function *pF, CallInst *pCI);
    //bool handleCalloc(Function *pF, CallInst *pCI);
    //bool handleRealloc(Function *pF, CallInst *pCI);
    //bool handleFree(Function *pF, CallInst *pCI);
    //void handleInstruction(Function *pF, Instruction *pI, CallInst *pCI);
    //void handleStoreInstruction(Function *pF, Value *pV, CallInst *pCI);
    //void handleLoadInstruction(Function *pF, Value *pV, CallInst *pCI);
  };
}

char AOSInputDetectPass::ID = 0;
static RegisterPass<AOSInputDetectPass> X("aos-i", "AOS malloc pass");

Pass *llvm::AOS::createAOSInputDetectPass() { return new AOSInputDetectPass(); }

bool AOSInputDetectPass::runOnBasicBlock(BasicBlock &BB) {
  bool basicblock_modified = false;

  std::list<CallInst*> callInsts;

  for (auto &I : BB) {
    if (CallInst *CI = dyn_cast<CallInst>(&I)) {
      Function *pF = CI->getCalledFunction();

			if (pF) {
				errs() << "pF->getName(): " << pF->getName() << "\n";
			}

      //if (pF && pF->getName() == "fread" ||
			//		pF->getName() == "fgetc" ||
			//		pF->getName() == "getc" ||
			//		pF->getName() == "fgets" ||
			//		pF->getName() == "getchar" ||
			//		pF->getName() == "gets" ||
			//		pF->getName() == "fgetwc" ||
			//		pF->getName() == "getwc" ||
			//		pF->getName() == "fgetws" ||
			//		pF->getName() == "getwchar" ||
			//		pF->getName() == "__isoc99_scanf" ||
			//		pF->getName() == "fscanf") {
					//pF->getName() == "sscanf" ||
					//pF->getName() == "vscanf" ||
					//pF->getName() == "vfscanf" ||
					//pF->getName() == "vsscanf" ||
					//pF->getName() == "wscanf" ||
					//pF->getName() == "fwscanf" ||
					//pF->getName() == "swscanf" ||
					//pF->getName() == "vwscanf" ||
					//pF->getName() == "vfwscanf" ||
					//pF->getName() == "vswscanf") {

      if (pF && pF->getName() == "malloc") {

        //callInsts.push_back(CI);
        basicblock_modified = handleInputFunc(pF, CI) || basicblock_modified;
			}
		}
  }

  //while (!callInsts.empty()) {
  //  CallInst *CI = callInsts.front();
  //  CI->eraseFromParent();
  //  callInsts.pop_front();
  //}

  return basicblock_modified;
}

bool AOSInputDetectPass::handleInputFunc(Function *pF, CallInst *pCI) {

	//errs() << "handleInputFunc: " << pF->getName() << "\n";

	errs() << "pCI: ";
	pCI->dump();

	if (!pCI->isTainted()) {
		errs() << "this is not tainted!\n";
	}

	pCI->setTainted(true);

	if (pCI->isTainted()) {
		errs() << "this is tainted!\n";
	}


	for (auto U : pCI->users()) {
		errs() << "pCI->users(): ";
		U->dump();

		if (auto Inst = dyn_cast<Instruction>(U)) {

			if (Inst->getOpcode() == Instruction::Store) {
				auto pSI = dyn_cast<StoreInst>(Inst);

				if (pSI->getValueOperand() == pCI) {
					auto Inst2 = dyn_cast<Instruction>(pSI->getPointerOperand());

					for (auto U2 : Inst2->users()) {
						if (auto Inst3 = dyn_cast<Instruction>(U2)) {
							errs() << "Inst3: ";
							Inst3->dump();

							if (Inst3->getOpcode() == Instruction::Load) {
								for (auto U3 : Inst3->users()) {
									errs() << "U3: ";
									U3->dump();
									auto Inst4 = dyn_cast<CallInst>(U3);

									for (auto op = U3->operands().begin();
														op != U3->operands().end(); ++op) {
										errs() << "op: ";
										dyn_cast<Value>(op)->dump();
									}

									Function * pF2 = Inst4->getCalledFunction();
									errs() << "pF2: " << pF2->getName() << "\n";

									for (auto arg = pF2->arg_begin();
													arg != pF2->arg_end(); ++arg) {
										errs() << "arg: ";
										arg->dump();

										for (auto U4 : arg->users()) {
											errs() << "arg->users(): ";
											U4->dump();
										}

									}

								}

							}
						}
					}
				}
			}
		}

	}


	//if (pF->getName() == "__isoc99_scanf") {
	//	errs() << "pCI: ";
	//	pCI->dump();

	//	auto arg = pCI->getArgOperand(0);
	//	errs() << "arg(0): ";
	//	arg->dump();

	//	arg = pCI->getArgOperand(1);
	//	errs() << "arg(1): ";
	//	arg->dump();

	//	auto pI = dyn_cast<Instruction>(arg);
	//	if (pI->getOpcode() == Instruction::Load) {
	//		auto pLI = dyn_cast<LoadInst>(pI);
	//		auto pI2 = dyn_cast<Instruction>(pLI->getPointerOperand());
	//		errs() << "pI2->dump(): ";
	//		pI2->dump();

	//		//if (pLI->getPointerOperand() == pI) {
	//			for (auto U : pI2->users()) {
	//				if (auto Inst = dyn_cast<Instruction>(U)) {
	//					errs() << "Inst->users(): ";
	//					Inst->dump();

	//					if (Inst->getOpcode() == Instruction::Store) {
	//						auto pSI = dyn_cast<StoreInst>(Inst);
	//						auto pI3 = dyn_cast<Instruction>(pSI->getPointerOperand());
	//						for (auto U2 : pI3->users()) {
	//							if (auto Inst2 = dyn_cast<Instruction>(U2)) {
	//								errs() << "Inst2->users(): ";
	//								Inst2->dump();
	//							}
	//						}
	//					}
	//				}
	//			}
	//		//}
	//	}


	//	for (auto U : arg->users()) {
	//		if (auto Inst = dyn_cast<Instruction>(U)) {
	//				errs() << "Inst->dump(): ";
	//				Inst->dump();
	//		}
	//	}
	//}



  //handleInstruction(pF, dyn_cast<Instruction>(pCI), pCI);

  //Value* arg = pCI->getArgOperand(0);
  //Value* args[] = {arg};

  ////args[0]->dump();
  ////std::cout << args[0]->getType();
  ////args[0]->mutateType(Type::getInt32Ty(C));
  ////args[0]->dump();
  ////std::cout << args[0]->getType();

  //LLVMContext &C = pF->getContext();
  //std::vector<Type*> paramTypes = {Type::getInt64Ty(C)};
  //Type *retType = Type::getInt8PtrTy(C);
  //FunctionType *FuncType = FunctionType::get(retType, paramTypes, false);
  ////Constant *malloc = pF->getParent()->getOrInsertFunction("aos_malloc", FuncType);
  //Constant *malloc = pF->getParent()->getOrInsertFunction("_Z10aos_mallocm", FuncType);

  //IRBuilder<> Builder(pCI->getNextNode());

  //auto newCI = Builder.CreateCall(malloc, args);

  //pCI->replaceAllUsesWith(newCI);

  //if (pCI->isTailCall())
  //  newCI->setTailCall();

  //newCI->getArgOperand(0)->mutateType(Type::getInt32Ty(C));

  return true;
}


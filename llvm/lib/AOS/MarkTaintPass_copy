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

#define DEBUG_TYPE "aos_mark_taint_pass"

namespace {
  class AOSMarkTaintPass : public BasicBlockPass {

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSMarkTaintPass() : BasicBlockPass(ID) {}

    bool runOnBasicBlock(BasicBlock &BB) override;

  private:  
    bool markTaintInfo(Instruction *pI, Value *pV);
    void handleCallInstruction(CallInst *pCI, Value *pV);
    void handleStoreInstruction(StoreInst *pSI);
    void handleLoadInstruction(LoadInst *pLI);
  };
}

char AOSMarkTaintPass::ID = 0;
static RegisterPass<AOSMarkTaintPass> X("aos-mark", "AOS mark taint pass");

Pass *llvm::AOS::createAOSMarkTaintPass() { return new AOSMarkTaintPass(); }

bool AOSMarkTaintPass::runOnBasicBlock(BasicBlock &BB) {
  bool basicblock_modified = false;

  std::list<CallInst*> callInsts;

  for (auto &I : BB) {
    if (CallInst *pCI = dyn_cast<CallInst>(&I)) {
      Function *pF = pCI->getCalledFunction();

			//if (pF) {
			//	errs() << "pF->getName(): " << pF->getName() << "\n";
			//}

      //if (pF && pF->getName() == "fread" ||
			//		pF->getName() == "fgetc" || o
			//		pF->getName() == "getc" || o
			//		pF->getName() == "fgets" || o
			//		pF->getName() == "getchar" || x
			//		pF->getName() == "gets" || x
			//		pF->getName() == "fgetwc" || x
			//		pF->getName() == "getwc" || x
			//		pF->getName() == "fgetws" || x
			//		pF->getName() == "getwchar" || x
			//		pF->getName() == "__isoc99_scanf" || o
			//		pF->getName() == "fscanf") { o
					//pF->getName() == "sscanf" || ?
					//pF->getName() == "vscanf" || ?
					//pF->getName() == "vfscanf" || ?
					//pF->getName() == "vsscanf" || ?
					//pF->getName() == "wscanf" || x
					//pF->getName() == "fwscanf" || x
					//pF->getName() == "swscanf" || ?
					//pF->getName() == "vwscanf" || ?
					//pF->getName() == "vfwscanf" || ?
					//pF->getName() == "vswscanf") {

      if (pF && pF->getName() == "__isoc99_scanf") {
				errs() << "Found scanf\n";

				Value* pV = pCI->getArgOperand(1);
        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), pV) || basicblock_modified;
			} else if (pF && (pF->getName() == "fscanf" ||
                  pF->getName() == "__isoc99_fscanf")) {
				errs() << "Found fscanf\n";

        int t = 0;
      	for (auto op = pCI->operands().begin();
						op != pCI->operands().end(); ++op) {
          if (t == 0 || t == 1)
            continue;

          Value* pV = pCI->getArgOperand(t);
          basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), pV) || basicblock_modified;

          t++;
        }
			} else if (pF && pF->getName() == "fgets") {
				errs() << "Found fgets\n";

				Value* pV = pCI->getArgOperand(0);
        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), pV) || basicblock_modified;
			} else if (pF && (pF->getName() == "fgetc" ||
                  pF->getName() == "_IO_getc")) {

				errs() << "Found fgetc\n";

        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), dyn_cast<Value>(pCI)) || basicblock_modified;
			} else if (pF && pF->getName() == "getc") {
				errs() << "Found getc\n";

        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), dyn_cast<Value>(pCI)) || basicblock_modified;
			} else if (pF && pF->getName() == "getcwd") {
				errs() << "Found getcwd\n";

				Value* pV = pCI->getArgOperand(1);
        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), pV) || basicblock_modified;
			} else if (pF && pF->getName() == "read") {
				errs() << "Found read\n";

				Value* pV = pCI->getArgOperand(1);
        basicblock_modified = markTaintInfo(dyn_cast<Instruction>(pCI), pV) || basicblock_modified;
      } 
		}
  }

  //while (!callInsts.empty()) {
  //  CallInst *pCI = callInsts.front();
  //  pCI->eraseFromParent();
  //  callInsts.pop_front();
  //}

  return basicblock_modified;
}

bool AOSMarkTaintPass::markTaintInfo(Instruction *pI, Value *pV) {

	// Mark taint itself
	errs() << "Mark this value as tainted";
	if (auto Inst = dyn_cast<Instruction>(pV))
		dyn_cast<Instruction>(pV)->setTainted(true);
	pV->dump();

	// Iterate users
	for (auto U: pV->users()) {
		//errs() << "U->dump(): ";
		//U->dump();

		if (auto pUI = dyn_cast<Instruction>(U)) {

			if (pUI == dyn_cast<Instruction>(pI))
				continue;

			if (CallInst *pCI = dyn_cast<CallInst>(pUI)) {
				//errs() << "Handle Call Inst!\n";

				// Look into the func
				handleCallInstruction(pCI, pV);
			} else if (pUI->getOpcode() == Instruction::Store) {
				auto pSI = dyn_cast<StoreInst>(pUI);

				if (pSI->getValueOperand() == pV) {
					//errs() << "Handle Store Inst!\n";

					handleStoreInstruction(pSI);
				}
			} else if (pUI->getOpcode() == Instruction::Load) {
				auto pLI = dyn_cast<LoadInst>(pUI);

				if (pLI->getPointerOperand() == pV) {
					//errs() << "Handle Load Inst!\n";

					markTaintInfo(pUI, dyn_cast<Value>(pUI));
					//handleLoadInstruction(pLI);
				}
			//} else {
      //  if (!pUI->isTainted())
    	//		markTaintInfo(pUI, dyn_cast<Value>(pUI));
      }
		}
	}

	if (auto pUI = dyn_cast<Instruction>(pV)) {
		if (pUI->getOpcode() == Instruction::Load) {
			auto pLI = dyn_cast<LoadInst>(pUI);

			//errs() << "Handle Load Inst (2)!\n";

			handleLoadInstruction(pLI);
		}
	}

	//for (auto U = pV->use_begin();
	//					U != pV->use_end(); U++) {
	//	errs() << "pUI->dump(): ";
	//	auto pUI = U->getUser();
	//	pUI->dump();

	//}

  return true;
}

void AOSMarkTaintPass::handleCallInstruction(CallInst *pCI, Value *pV) {
	unsigned int arg_n = 0;

	for (auto op = pCI->operands().begin();
						op != pCI->operands().end(); ++op) {

		if (dyn_cast<Value>(op) == pV)
			break;

		arg_n++;
	}
	
	Function *pF = pCI->getCalledFunction();
	//errs() << "pF->getName: " << pF->getName() << "\n";

	// libc func.
	if (arg_n > pF->arg_size() - 1)
		return;

	//errs() << "arg_size: " << pF->arg_size() << "\n";

	auto arg = pF->arg_begin();
	for (unsigned int i=0; i<arg_n; i++) {
		arg++;
	}

	//errs() << "arg->dump(): ";
	//arg->dump();

	if (arg)
		markTaintInfo(dyn_cast<Instruction>(pCI), arg);
}

void AOSMarkTaintPass::handleStoreInstruction(StoreInst *pSI) {
	Value *pV = pSI->getPointerOperand();

  for (auto U : pV->users()) {
    if (auto pUI = dyn_cast<Instruction>(U)) {

      if (pUI->getOpcode() == Instruction::Load) {
        auto pLI = dyn_cast<LoadInst>(pUI);

        if (pLI->getPointerOperand() == pV &&
					!dyn_cast<Instruction>(pV)->isTainted())
					markTaintInfo(dyn_cast<Instruction>(pLI), dyn_cast<Value>(pLI));
      }
    }
  }
}

void AOSMarkTaintPass::handleLoadInstruction(LoadInst *pLI) {
	Value *pV = pLI->getPointerOperand();

	if (!dyn_cast<Instruction>(pV)->isTainted())
		markTaintInfo(dyn_cast<Instruction>(pLI), pV);
}

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

#define DEBUG_TYPE "aos_malloc_pass"

namespace {
  class AOSMallocPass : public BasicBlockPass {

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSMallocPass() : BasicBlockPass(ID) {}

    bool runOnBasicBlock(BasicBlock &BB) override;

  private:  
    bool handleMalloc(Function *pF, CallInst *pCI);
    bool handleCalloc(Function *pF, CallInst *pCI);
    bool handleRealloc(Function *pF, CallInst *pCI);
    bool handleFree(Function *pF, CallInst *pCI);
    void handleInstruction(Function *pF, Instruction *pI, CallInst *pCI);
    void handleStoreInstruction(Function *pF, Value *pV, CallInst *pCI);
    void handleLoadInstruction(Function *pF, Value *pV, CallInst *pCI);
  };
}

char AOSMallocPass::ID = 0;
static RegisterPass<AOSMallocPass> X("aos-m", "AOS malloc pass");

Pass *llvm::AOS::createAOSMallocPass() { return new AOSMallocPass(); }

bool AOSMallocPass::runOnBasicBlock(BasicBlock &BB) {
  bool basicblock_modified = false;

  std::list<CallInst*> callInsts;

  for (auto &I : BB) {
    if (CallInst *CI = dyn_cast<CallInst>(&I)) {
      Function *pF = CI->getCalledFunction();

      if (pF && pF->getName() == "malloc") {
        callInsts.push_back(CI);
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "calloc") {
        callInsts.push_back(CI);
        basicblock_modified = handleCalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "realloc") {
        callInsts.push_back(CI);
        basicblock_modified = handleRealloc(pF, CI) || basicblock_modified;     
      } else if (pF && pF->getName() == "free") {
        callInsts.push_back(CI);
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_Znwm") { // new
        callInsts.push_back(CI);
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_Znam") { // new[]
        callInsts.push_back(CI);
        basicblock_modified = handleMalloc(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_ZdlPv") { //delete
        callInsts.push_back(CI);
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      } else if (pF && pF->getName() == "_ZdaPv") { // delete[]
        callInsts.push_back(CI);
        basicblock_modified = handleFree(pF, CI) || basicblock_modified;
      }
    }
  }

  while (!callInsts.empty()) {
    CallInst *CI = callInsts.front();
    CI->eraseFromParent();
    callInsts.pop_front();
  }

  return basicblock_modified;
}

bool AOSMallocPass::handleMalloc(Function *pF, CallInst *pCI) {

  handleInstruction(pF, dyn_cast<Instruction>(pCI), pCI);

  Value* arg = pCI->getArgOperand(0);
  Value* args[] = {arg};

  //args[0]->dump();
  //std::cout << args[0]->getType();
  //args[0]->mutateType(Type::getInt32Ty(C));
  //args[0]->dump();
  //std::cout << args[0]->getType();

  LLVMContext &C = pF->getContext();
  std::vector<Type*> paramTypes = {Type::getInt64Ty(C)};
  Type *retType = Type::getInt8PtrTy(C);
  FunctionType *FuncType = FunctionType::get(retType, paramTypes, false);
  //Constant *malloc = pF->getParent()->getOrInsertFunction("aos_malloc", FuncType);
  Constant *malloc = pF->getParent()->getOrInsertFunction("_Z10aos_mallocm", FuncType);

  IRBuilder<> Builder(pCI->getNextNode());

  auto newCI = Builder.CreateCall(malloc, args);

  pCI->replaceAllUsesWith(newCI);

  if (pCI->isTailCall())
    newCI->setTailCall();

  //newCI->getArgOperand(0)->mutateType(Type::getInt32Ty(C));

  return true;
}

bool AOSMallocPass::handleCalloc(Function *pF, CallInst *pCI) {

  handleInstruction(pF, dyn_cast<Instruction>(pCI), pCI);

  auto arg0 = pCI->getArgOperand(0);
  auto arg1 = pCI->getArgOperand(1);
  Value* args[] = {arg0, arg1};

  LLVMContext &C = pF->getContext();
  std::vector<Type*> paramTypes = {Type::getInt64Ty(C), Type::getInt64Ty(C)};
  Type *retType = Type::getInt8PtrTy(C);
  FunctionType *FuncType = FunctionType::get(retType, paramTypes, false);
  //Constant *calloc = pF->getParent()->getOrInsertFunction("aos_calloc", FuncType);
  Constant *calloc = pF->getParent()->getOrInsertFunction("_Z10aos_callocmm", FuncType);

  IRBuilder<> Builder(pCI->getNextNode());

  auto newCI = Builder.CreateCall(calloc, args);

  pCI->replaceAllUsesWith(newCI);

  if (pCI->isTailCall())
    newCI->setTailCall();

  return true;
}

bool AOSMallocPass::handleRealloc(Function *pF, CallInst *pCI) {

  handleInstruction(pF, dyn_cast<Instruction>(pCI), pCI);

  auto arg0 = pCI->getArgOperand(0);
  auto arg1 = pCI->getArgOperand(1);
  Value* args[] = {arg0, arg1};

  LLVMContext &C = pF->getContext();
  std::vector<Type*> paramTypes = {Type::getInt8PtrTy(C), Type::getInt64Ty(C)};
  Type *retType = Type::getInt8PtrTy(C);
  FunctionType *FuncType = FunctionType::get(retType, paramTypes, false);
  //Constant *realloc = pF->getParent()->getOrInsertFunction("aos_realloc", FuncType);
  Constant *realloc = pF->getParent()->getOrInsertFunction("_Z11aos_reallocPvm", FuncType);

  IRBuilder<> Builder(pCI->getNextNode());

  auto newCI = Builder.CreateCall(realloc, args);

  pCI->replaceAllUsesWith(newCI);

  if (pCI->isTailCall())
    newCI->setTailCall();

  return true;
}

bool AOSMallocPass::handleFree(Function *pF, CallInst *pCI) {
  auto arg = pCI->getArgOperand(0);
  Value* args[] = {arg};

  LLVMContext &C = pF->getContext();
  std::vector<Type*> paramTypes = {Type::getInt8PtrTy(C)};
  Type *retType = Type::getVoidTy(C);
  FunctionType *FuncType = FunctionType::get(retType, paramTypes, false);
  //Constant *free = pF->getParent()->getOrInsertFunction("aos_free", FuncType);
  Constant *free = pF->getParent()->getOrInsertFunction("_Z8aos_freePv", FuncType);

  IRBuilder<> Builder(pCI->getNextNode());

  auto newCI = Builder.CreateCall(free, args);

  pCI->replaceAllUsesWith(newCI);

  if (pCI->isTailCall())
    newCI->setTailCall();

  return true;
}

void AOSMallocPass::handleInstruction(Function *pF, Instruction *pI, CallInst *pCI) {

  //if (pI->getParent() != pCI->getParent()) {
  //  errs() << "Two different basic blocks!\n";
  //  errs() << "pI->getParent(): " << pI->getParent()->getName() << "\n";
  //  errs() << "pCI->getParent(): " << pCI->getParent()->getName() << "\n";

  //  SmallVector<BasicBlock *, 8> Preds(pred_begin(pI->getParent()), pred_end(pI->getParent()));
  //  
  //  bool check = false;
  //  for (BasicBlock *Pred : Preds) {
  //    if (Pred == pCI->getParent()) {
  //      check = true;
  //      break;
  //    }
  //  }

  //  if (check == false) {
  //    errs() << "Couldn't find !\n";
  //  }
  //}



  std::vector<CallInst *> callInsts;

  for (auto U : pI->users()) {
    if (auto Inst = dyn_cast<Instruction>(U)) {

      if (auto pGI = dyn_cast<GetElementPtrInst>(U)) {
        handleInstruction(pF, Inst, pCI);
      } else if (auto pBI = dyn_cast<BitCastInst>(U)) {
        handleInstruction(pF, Inst, pCI);
      } else if (auto pTI = dyn_cast<CallInst>(U)) {
        //if (pTI->getCalledFunction()->getName().find("pa.pacda") !=
        //        std::string::npos) {
        if (pTI->getCalledFunction() != nullptr && 
              pTI->getCalledFunction()->getName().find("pa.pacda") !=
              std::string::npos) {
          //errs() << "Found PACDA! " << *pTI << "\n";

          handleInstruction(pF, Inst, pCI);

          pTI->replaceAllUsesWith(pTI->getOperand(0));
          callInsts.push_back(pTI);
          //pTI->eraseFromParent();
        }
      } else if (Inst->getOpcode() == Instruction::Store) {
        auto pSI = dyn_cast<StoreInst>(Inst);

        if (pSI->getValueOperand() == pI)
          handleStoreInstruction(pF, pSI->getPointerOperand(), pCI);
      }
    }
  }

  for (auto it = callInsts.begin(); it != callInsts.end(); ++it)
    (*it)->eraseFromParent();  
}

void AOSMallocPass::handleStoreInstruction(Function *pF, Value *pV, CallInst *pCI) {
  for (auto U : pV->users()) {
    if (auto Inst = dyn_cast<Instruction>(U)) {

      if (auto pGI = dyn_cast<GetElementPtrInst>(U)) {
        assert(false);
      } else if (auto pBI = dyn_cast<BitCastInst>(U)) {
        handleStoreInstruction(pF, pBI, pCI);
      } else if (Inst->getOpcode() == Instruction::Load) {
        auto pLI = dyn_cast<LoadInst>(Inst);

        if (pLI->getPointerOperand() == pV)
          handleLoadInstruction(pF, pLI, pCI);
      }
    }
  }
}

void AOSMallocPass::handleLoadInstruction(Function *pF, Value *pV, CallInst *pCI) {

  std::vector<CallInst *> callInsts;

  for (auto U : pV->users()) {
    if (auto Inst = dyn_cast<Instruction>(U)) {

      if (auto pTI = dyn_cast<CallInst>(U)) {
        if (pTI->getCalledFunction()->getName().find("pa.autda") !=
              std::string::npos) {
          //errs() << "Found AUTDA! " << *pTI << "\n";

          auto arg = pTI->getOperand(0);
          //Value *op2 = pCI;

          std::vector<Type *> arg_type;
          arg_type.push_back(arg->getType());

          IRBuilder<> Builder(pTI);

          auto autm = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_autm, arg_type);

          //auto authenticated = Builder.CreateCall(autm, {arg, pTI->getOperand(1), pCI}, "");
          auto authenticated = Builder.CreateCall(autm, {arg}, "");

          //errs() << "Authenticated! " << *authenticated << "\n";

          pTI->replaceAllUsesWith(authenticated);

          callInsts.push_back(pTI);
        }
      }
    }
  }

  for (auto it = callInsts.begin(); it != callInsts.end(); ++it)
    (*it)->eraseFromParent();
    ////if (pTI->getParent() != pCI->getParent()) {
    ////  auto Inst = pTI->getParent()->getFirstNonPHI();
    ////  IRBuilder<> Builder(Inst);
    ////  //errs() << "Two different basic blocks!\n";

    ////  auto phi = Builder.CreatePHI(pCI->getType(), 1, "myPHI");

    ////  SmallVector<BasicBlock *, 8> Preds(pred_begin(pTI->getParent()), pred_end(pTI->getParent()));
    ////  for (BasicBlock *Pred : Preds) {
    ////    phi->addIncoming(pCI, Pred);
    ////  }

    ////  //phi->addIncoming(pCI, pCI->getParent());
    ////  op2 = phi;

    ////  //errs() << "myPHI: " << *phi << "\n";
    ////  //PHINode *InputV = PHINode::Create(
    ////  //    V->getType(), 1, V->getName() + Twine(".") + BB.getName(),
    ////  //    &IncomingBB->front());      
    ////}
}


//void AOSMallocPass::handleInstruction(Function *pF, Instruction *pI) {
//
//  //errs() << "Inst: ";
//  //pI->dump();
//
//  for (auto U : pI->users()) {
//    if (auto Inst = dyn_cast<Instruction>(U)) {
//
//      //errs() << "pI's users: ";
//      //errs() << *Inst << "\n";
//
//      if (auto pGI = dyn_cast<GetElementPtrInst>(U)) {
//          handleInstruction(pF, Inst);
//      } else if (auto pBI = dyn_cast<BitCastInst>(U)) {
//          handleInstruction(pF, Inst);
//      } else {
//        auto pSI = dyn_cast<StoreInst>(Inst);
//        auto pLI = dyn_cast<LoadInst>(Inst);
//
//        auto pCI = dyn_cast<CallInst>(Inst);
//
//        switch (Inst->getOpcode()) {
//          case Instruction::Store:
//            if (pSI->getPointerOperand() == pI) {
//              //errs() << "[Store] Pointer Operand == pI\n";
//              handleStoreInstruction(pF, pSI);
//            } else if (pSI->getValueOperand() == pI) {
//              handleStoreValue(pF, pSI);
//            }
//            //else {
//            //  errs() << "pSI->dump():";
//            //  pSI->dump();
//            //}
//
//            break;
//          case Instruction::Load:
//            if (pLI->getPointerOperand() == pI)
//              //errs() << "[Load] Pointer Operand == pI\n";
//              handleLoadInstruction(pF, pLI);
//            //else {
//            //  errs() << "pLI->dump():";
//            //  pLI->dump();
//            //}
//
//            assert(pLI != pI);
//
//            break;
//          //case Instruction::Call:
//          //  if (pCI->getCalledFunction()->getName().find("pa.pacda") != std::string::npos) {
//          //    errs() << "Found PACDA! ";
//          //    errs() << pCI->getCalledFunction()->getName() << "\n";
//
//          //    pCI->replaceAllUsesWith(pCI->getOperand(0));
//          //    //pCI->removeFromParent();
//          //  } else if (pCI->getCalledFunction()->getName().find("pa.autda") != std::string::npos) {
//          //    errs() << "Found AUTDA! ";
//          //    errs() << pCI->getCalledFunction()->getName() << "\n";
//
//          //    pCI->replaceAllUsesWith(pCI->getOperand(0));
//          //  }
//          //  break;
//          default:
//            break;
//        }
//      }
//    }
//  }
//}
//
//bool AOSMallocPass::handleStoreInstruction(Function *pF, StoreInst *pSI) {
//  //errs() << "[Store] Pointer Operand == pI\n";
//  //errs() << "Store: ";
//  //pSI->dump();
//
//  auto arg = pSI->getPointerOperand();
//
//  //arg->getType()->dump();
//
//  std::vector<Type *> arg_type;
//  arg_type.push_back(arg->getType());
//
//  IRBuilder<> Builder(pSI);
//  auto autm = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_autm, arg_type);
//
//  auto authenticated = Builder.CreateCall(autm, {arg}, "");
//
//  pSI->setOperand(1, authenticated);
//
//  return true;
//}
//
//bool AOSMallocPass::handleLoadInstruction(Function *pF, LoadInst *pLI) {
//  //errs() << "Load: ";
//  //pLI->dump();
//
//  auto arg = pLI->getPointerOperand();
//
//  //arg->getType()->dump();
//
//  std::vector<Type *> arg_type;
//  arg_type.push_back(arg->getType());
//
//  IRBuilder<> Builder(pLI);
//  auto autm = Intrinsic::getDeclaration(pF->getParent(), Intrinsic::aos_autm, arg_type);
//
//  auto authenticated = Builder.CreateCall(autm, {arg}, "");
//
//  pLI->setOperand(0, authenticated);
//
//  return true;
//}
//
//bool AOSMallocPass::handleStoreValue(Function *pF, StoreInst *pSI) {
//  errs() << "[Store] Value == pI!\n";
//  auto V = pSI->getPointerOperand();
//
//  for (auto U : V->users()) {
//    if (auto Inst = dyn_cast<Instruction>(U)) {
//      errs() << "V's users: ";
//      errs() << *Inst << "\n";
//
//      if (auto pGI = dyn_cast<GetElementPtrInst>(U)) {
//        errs() << "  In StoreValue, pGI found\n";
//      } else if (auto pBI = dyn_cast<BitCastInst>(U)) {
//        errs() << "  In StoreValue, pBI found\n";
//      }
//
//      auto pSI = dyn_cast<StoreInst>(Inst);
//      auto pLI = dyn_cast<LoadInst>(Inst);
//
//      switch (Inst->getOpcode()) {
//        case Instruction::Store:
//          if (pSI->getValueOperand() == V) {
//            errs() << "  [Store] Value Operand == V\n";
//            assert(false);
//          }
//            //handleStoreInstruction(pF, pSI);
//          break;
//        case Instruction::Load:
//          if (pLI->getPointerOperand() == V)
//            errs() << "  [Load] Pointer Operand == V\n";
//            //handleLoadInstruction(pF, pLI);
//          break;
//        default:
//          break;
//      }
//    }
//  }
//
//  return true;
//}

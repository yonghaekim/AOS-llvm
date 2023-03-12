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
#include "llvm/AOS/AOSPointerAliasPass.h"
#include <iostream>

using namespace llvm;
using namespace AOS;
using namespace std;

//#define DEBUG_TYPE "aos_mark_taint_pass"

//namespace {
  class AOSMarkTaintPass : public ModulePass {

  typedef AOSPointerAliasPass::AOSNode AOSNode;
  typedef AOSPointerAliasPass::AOSAlias AOSAlias;

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSMarkTaintPass() : ModulePass(ID) {}

    map<Value *, AOSNode *> value_map; 
    set<Value *> ex_input_set;
		list<AOSAlias *> work_list;
		set<Value *> visit_val_set;
		set<AOSNode *> visit_node_set;

    AOSNode *root_node;
  	LLVMContext *C;

    bool runOnModule(Module &M) override;
		void getAnalysisUsage(AnalysisUsage &AU) const;
    map<Value *, AOSPointerAliasPass::AOSNode *> getValueMap();

  private:  
		void handleGlobalVariables(Module &M);
		void handleCmdLineArguments(Module &M);
		void handleExInputFunc(Module &M);
		void handleInstructions(Module &M);
    bool doReachabilityTest(AOSNode *node);
    void taintFree(AOSNode *node);
    void handleStruct(Value *pV);
    bool IsStructTyWithArray(Type *ty);
    void handleAOSNode(AOSNode *node);
    void doTaintPropagation(Value *pV);
		Value *getArgument(Value *pI, Value *pV);
		void printNode(AOSNode *node);
		void handleIntrinsicFunction(Function *pF, Value *pV, CallInst *pCI);
  };
//}


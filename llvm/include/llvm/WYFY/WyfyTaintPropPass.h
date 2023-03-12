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
#include "llvm/WYFY/WyfyPtrAliasPass.h"
#include <iostream>

using namespace llvm;
using namespace WYFY;
using namespace std;

//#define DEBUG_TYPE "aos_mark_taint_pass"

//namespace {
  class WyfyTaintPropPass : public ModulePass {

  typedef WyfyPtrAliasPass::WyfyNode WyfyNode;
  typedef WyfyPtrAliasPass::WyfyAlias WyfyAlias;

  public:
    static char ID; // Pass identification, replacement for typeid
    WyfyTaintPropPass() : ModulePass(ID) {}

    map<Value *, WyfyNode *> value_map; 
    set<Value *> ex_input_set;
		list<WyfyAlias *> work_list;
		set<Value *> visit_val_set;
		set<WyfyNode *> visit_node_set;

    WyfyNode *root_node;
  	LLVMContext *C;

    bool runOnModule(Module &M) override;
		void getAnalysisUsage(AnalysisUsage &AU) const;
    map<Value *, WyfyPtrAliasPass::WyfyNode *> getValueMap();

  private:  
		void handleGlobalVariables(Module &M);
		void handleCmdLineArguments(Module &M);
		void handleExInputFunc(Module &M);
		void handleInstructions(Module &M);
    bool doReachabilityTest(WyfyNode *node);
    void taintFree(WyfyNode *node);
    void handleStruct(Value *pV);
    bool IsStructTyWithArray(Type *ty);
    void handleWyfyNode(WyfyNode *node);
    void doTaintPropagation(Value *pV);
		Value *getArgument(Value *pI, Value *pV);
		void printNode(WyfyNode *node);
		void handleIntrinsicFunction(Function *pF, Value *pV, CallInst *pCI);
  };
//}


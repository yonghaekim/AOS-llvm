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
#include "llvm/AOS/AOSMarkTaintPass.h"
#include "llvm/Support/CommandLine.h"
#include <regex>
#include <map>
#include <iostream>

using namespace llvm;
using namespace AOS;

//#define DEBUG_TYPE "aos_reach_test_pass"

//namespace {
  class AOSReachTestPass : public ModulePass {

  typedef AOSPointerAliasPass::AOSNode AOSNode;
  typedef AOSPointerAliasPass::AOSAlias AOSAlias;

  public:
    static char ID; // Pass identification, replacement for typeid
    AOSReachTestPass() : ModulePass(ID) {}

    map<Value *, AOSNode *> value_map; 
    set<AOSNode *> visit_set;
		list<Value *> gv_list;
		//list<pair<Function *, AllocaInst *>> alloc_list;
		//list<pair<Function *, CallInst *>> malloc_list;
		//list<pair<Function *, CallInst *>> calloc_list;
		//list<pair<Function *, CallInst *>> realloc_list;
		//list<pair<Function *, CallInst *>> free_list;
		list<Instruction *> inst_list;
		//list<AllocaInst *> array_list;
		//list<AllocaInst *> struct_list;
		//list<CallInst *> malloc_list;
		//list<CallInst *> calloc_list;
		//list<CallInst *> realloc_list;
		//list<CallInst *> free_list;

		set<AOSNode *> taint_nodes;
		map<Type *, set<unsigned>> taint_indices;
		map<Type *, set<unsigned>> signed_indices;
		set<Value *> sign_set;
		list<vector<Value *>> indices_list;

		LLVMContext *C;
		const DataLayout *DL;

    bool runOnModule(Module &M) override;
		void getAnalysisUsage(AnalysisUsage &AU) const;

    bool Baseline = false;
    bool AOS = false;
    bool WYFY_C = false;
    bool WYFY_F = false;
    bool WYFY_FT = false;

    unsigned statNumGV = 0;
    unsigned statNumGVSigned = 0;

    unsigned statNumAI = 0;
    unsigned statNumAISigned = 0;

    unsigned statNumCI = 0;
    unsigned statNumCISigned = 0;

    unsigned temp_cnt = 0;

  private:  
    void handleCmdLineArguments(Module &M);
		bool handleGlobalVariables(Module &M);
		//void handleCmdLineArguments(Module &M);
		bool handleInstructions(Module &M);
    bool handlePtrToInts(Module &M);
		bool handleStructs(Module &M);
    bool doReachabilityTest(AOSNode *node);
		bool handleAlloca(Function *pF, AllocaInst *pAI);
		bool handleGlobalVariable(Function *pF, GlobalVariable *pGV);
		bool handleDealloc(Function *pF, ReturnInst *pRI);
		bool handleMalloc(Function *pF, Instruction *pI);
		bool handleFree(Function *pF, Instruction *pI);
		//bool handleStruct(Function *pF, AllocaInst *pAI);
		//bool handleStruct(Function *pF, GlobalVariable *pGV);
		//bool handleStruct(Function *pF, Value *pV, Type *ty);
		bool handleStruct(Function *pF, Value *pV, set<Type *> type_set);
		void findTaintIndices(Function *pF, Value *pV);
		bool IsArrayTy(Type *ty);
		bool IsStructTy(Type *ty);
    bool IsStructTyWithArray(Type *ty);
    set<Type *> getStructTypes(Type *ty, set<Type *> type_set);
		//Type *GetStructTy(Type *ty);
		//unsigned GetStartIdx(Function *pF, Type *ty);
		void printNode(AOSNode *node);
		void init(Module &M);
		void handleElement(Function *pF, Value *pV, Type *ty, list<vector<Value *>> indices_list, bool isGV, bool isCI);
    void buildTypeString(const Type *T, llvm::raw_string_ostream &O);
    uint64_t getTypeIDFor(const Type *T);
    Constant *getTypeIDConstantFrom(const Type &T, LLVMContext &C);
		void insertSign(Function *pF, GetElementPtrInst *pGEP, Type *ty);
    void replaceUsesInFunction(Value *pV, Value *pNew, Function *pF);
  };
//}


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
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/WYFY/WYFY.h"
//#include "llvm/WYFY/WyfyBBCounterPass.h"
#include <iostream>

using namespace llvm;
using namespace WYFY;
using namespace std;

//#define DEBUG_TYPE "aos_mark_taint_pass"

//namespace {
  class WyfyPtrAliasPass : public ModulePass {

  public:
    static char ID; // Pass identification, replacement for typeid
    WyfyPtrAliasPass() : ModulePass(ID) {}

		class WyfyAlias;
		class WyfyNode;

    class WyfyAlias {
    public:
      Value *ptr;
      WyfyNode *node;

      WyfyAlias () { }

      WyfyAlias (Value *_ptr, WyfyNode *_node) {
        ptr = _ptr;
				node = _node;
      }

      Value *getPtr() { return ptr; }

      WyfyNode *getNode() { return node; }

      void setNode(WyfyNode *_node) { node = _node; }
    };

    class WyfyNode {
    public:
			Type *ty;
			unsigned start_idx;
			list<Value *> indices;
			map<Value *, WyfyAlias *> aliases;

			set<WyfyNode *> children;
			set<WyfyNode *> parents;

			WyfyNode *mem_user;
			set<WyfyNode *> mem_edges;

			bool tainted;

      WyfyNode () {
				start_idx = 0;
				mem_user = nullptr;
				tainted = false;
			}

			void setType(Type *_ty) {
				ty = _ty;

				if (PointerType *_pty = dyn_cast<PointerType>(_ty)) {
					Type *element_ty = _pty->getElementType();
					start_idx = 0;

					while (element_ty->isArrayTy()) {
						element_ty = element_ty->getArrayElementType();
						start_idx++;
					}
				} else {
					_ty->dump();
					assert(false);
				}
				//errs() << "For this type: "; _ty->dump();
				//errs() << "start_idx: " << start_idx << "\n";
			}

			void addChild(WyfyNode *node) {
				children.insert(node);
			}

			void addParent(WyfyNode *node) {
				parents.insert(node);
			}

			void removeChild(WyfyNode *node) {
				children.erase(node);
			}

			void removeParent(WyfyNode *node) {
				parents.erase(node);
			}

			bool findAlias(Value *ptr) {
				return (aliases[ptr] == NULL) ? false : true;
			}

			void addAlias(WyfyAlias *alias) {
        if (ty != alias->getPtr()->getType()) {
          alias->getPtr()->dump();
          alias->getPtr()->getType()->dump();
          ty->dump();
          assert(false);
        }

        assert(ty == alias->getPtr()->getType());
				aliases[alias->getPtr()] = alias;
			}

			void setMemUser(WyfyNode *node) {
				if (node != nullptr)
					assert(mem_user == nullptr);
				mem_user = node;
			}

			void addMemEdge(WyfyNode *node) {
				mem_edges.insert(node);
			}

			void removeMemEdge(WyfyNode *node) {
				mem_edges.erase(node);
			}

			WyfyNode* findNodeWithTy(Type *_ty) {
        if (ty == _ty)
          return this;

        int temp_cnt = 0;
        //errs() << "[1]children.size: " << children.size() << "\n";
				for (auto it = children.begin(); it != children.end(); it++) {
          //errs() << "[1]temp_cnt: " << temp_cnt++ << "\n";
          //(*it)->ty->dump();

					if ((*it)->ty == _ty)
            return (*it);
        }

				//errs() << "Return nullptr!\n";
				return nullptr;		
			}

			WyfyNode* findNodeWithTy(Type *_ty, list<Value *> new_indices) {
				if (ty == _ty)
					return this;

        int temp_cnt = 0;
        //errs() << "children.size: " << children.size() << "\n";
        //errs() << "new_indices.size: " << new_indices.size() << "\n";
				for (auto it = children.begin(); it != children.end(); it++) {
          //errs() << "temp_cnt: " << temp_cnt++ << "\n";
          //(*it)->ty->dump();

					if ((*it)->ty == _ty) {
            unsigned i;
						unsigned size = (*it)->indices.size();

						if (size != new_indices.size())
							continue;

						auto itr_a = (*it)->indices.begin();
						auto itr_b = new_indices.begin();
						//errs() << "size: " << size << "\n";

						if (size < start_idx + 1)
							return (*it);

						for (i=0; i<size; i++, itr_a++, itr_b++) {
							if (i < start_idx)
								continue;

							if ((*itr_a) != (*itr_b))
								break;
						}

						if (i == size)
							return (*it);
					}
				}

				//errs() << "Return nullptr!\n";
				return nullptr;		
			}

			bool isTainted() { return tainted; }

			void setTainted(bool b) { tainted = b; }
		};

    list<WyfyAlias *> work_list;
		list<Function *> func_list;
		list<Function *> uncalled_list;
		map<Value *, WyfyNode *> value_map;
		WyfyNode *root_node;
		set<WyfyNode *> freed_nodes;
    map<Instruction *, list<list<Operator *>>> inst_map;

    bool runOnModule(Module &M) override;
		void getAnalysisUsage(AnalysisUsage &AU) const;
    WyfyPtrAliasPass::WyfyNode* getRootNode();
    map<Value *, WyfyPtrAliasPass::WyfyNode *> getValueMap();
    list<Function *>  getUncalledList();

		LLVMContext *C;
		const DataLayout *DL;

  private:  
		//void getFunctionsFromCallGraph(Module &M);
    void handleLoadPtrOperand(Value *pV);
    void handleLoads(Module &M);
		void handleGlobalVariables(Module &M);
		void handleOperators(GlobalVariable *pV);
    void getUsersOfGV(Value *pV, list<Operator *> op_list);
		void handleGlobalPointers(Module &M);
		void handleInstructions(BasicBlock *BB);
		void handleArguments(Function *pF);
		void getFunctionsFromCallGraph(Module &M);
		//void handleCmdLineArguments(Module &M);
		void getPtrAliases(WyfyAlias *alias);
		void mergeNode(WyfyNode *dst, WyfyNode *src);
    Value *getArgument(Value *pI, Value *pV);
    bool index_compare(std::list<Value*> idx_list_a, std::list<Value*> idx_list_b, int start_idx);
		void dump();
		void printNode(WyfyNode *node);
    void replaceGEPOps(Module &M);
    bool IsStructTy(Type *ty);
		void init(Module &M);
  };
//}


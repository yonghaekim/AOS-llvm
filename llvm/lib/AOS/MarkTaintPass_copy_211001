#include "llvm/AOS/AOSMarkTaintPass.h"

char AOSMarkTaintPass::ID = 0;
static RegisterPass<AOSMarkTaintPass> X("aos-mark", "AOS mark taint pass");

Pass *llvm::AOS::createAOSMarkTaintPass() { return new AOSMarkTaintPass(); }

bool AOSMarkTaintPass::runOnModule(Module &M) {
	errs() << "Start taint propagation pass!\n";
  AOSPointerAliasPass &MT = getAnalysis<AOSPointerAliasPass>();
	root_node = MT.getRootNode();
  value_map = MT.getValueMap();
  list<Function *> uncalled_list = MT.getUncalledList();

  handleCmdLineArguments(M);

	for (auto it = uncalled_list.begin(); it != uncalled_list.end(); it++) {
		Function *pF = (*it);

		for (auto arg = pF->arg_begin(); arg != pF->arg_end(); arg++) {
			if (arg->getType()->isPointerTy()) {
				if (AOSNode *node = value_map[arg])
					handleAOSNode(node);
			} else {
        doTaintPropagation(arg);
			}
		}
	}

  handleInstructions(M);




	unsigned cnt = 0;
	//errs() << "Print ExInputSet!\n";
	for (auto it = ex_input_set.begin(); it != ex_input_set.end(); it++) {
		(*it)->dump();
		cnt++;
	}
	errs() << "Total #: "<< cnt << "\n";

  return false;
}

void AOSMarkTaintPass::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
  AU.addRequired<AOSPointerAliasPass>();
}

map<Value *, AOSPointerAliasPass::AOSNode *> AOSMarkTaintPass::getValueMap() {
	return value_map;
}

void AOSMarkTaintPass::handleCmdLineArguments(Module &M) {
	// TODO Need to maintain aliases for the arguments...
  for (auto &F : M) {
		if (&F && F.getName() == "main") {
			unsigned t = 0;
			for (auto arg = F.arg_begin(); arg != F.arg_end(); arg++) {
				// argc
				if (t == 0) {
					for (auto pU: arg->users()) {
						if (StoreInst *pSI = dyn_cast<StoreInst>(pU)) {
							if (pSI->getValueOperand() == arg) {				
								auto ptrOp = pSI->getPointerOperand();

								for (auto pUb: ptrOp->users()) {
									if (LoadInst *pLI = dyn_cast<LoadInst>(pUb)) {
										doTaintPropagation(pLI);
									}
								}

								break;
							}
						}
					}
				// argv
				} else if (t == 1) {
          AOSNode *node = value_map[arg];
          assert(node != nullptr);

          for (auto alias: node->aliases) {
            Value *pV = alias.first;

            for (auto pU: pV->users()) {
              if (GetElementPtrInst *pGEP = dyn_cast<GetElementPtrInst>(pU)) {
                assert (pGEP->getPointerOperand() == pV);

                for (auto pUb: pGEP->users()) {
                  if (LoadInst *pLI = dyn_cast<LoadInst>(pUb)) {
                    if (AOSNode *_node = value_map[pGEP])
                      handleAOSNode(_node);
                  }
                }
              }
            }
          }
					//for (auto pU: arg->users()) {
					//	if (StoreInst *pSI = dyn_cast<StoreInst>(pU)) {
					//		if (pSI->getValueOperand() == arg) {
					//			auto ptrOp = pSI->getPointerOperand();

					//			for (auto pUb: ptrOp->users()) {
					//				if (LoadInst *pLI = dyn_cast<LoadInst>(pUb)) {
					//					doTaintPropagation(pLI);
					//				}
					//			}

					//			break;
					//		}
					//	}
					//}
				} else {
					break;
				}

				t++;
			}
		}
	}
}

void AOSMarkTaintPass::handleInstructions(Module &M) {
  for (auto &F : M) {
    for (auto &BB : F) {
      for (auto &I : BB) {
				if (CallInst *pCI = dyn_cast<CallInst>(&I)) {
					Function *pF = pCI->getCalledFunction();
					//errs() << "  @@pF->name(): " << pF->getName() << "\n";
					if (pF && (pF->getName() == "__isoc99_scanf" ||
											pF->getName() == "scanf" ||
											pF->getName() == "wscanf")) {
						//errs() << "Found " << pF->getName() << ": ";
						//U->dump();
						// Except arg 0
						int t = 0;
						for (auto arg = pCI->arg_begin(); arg != pCI->arg_end(); ++arg) {
							if (t != 0) {

								Value *pV = dyn_cast<Value>(arg);
								ex_input_set.insert(dyn_cast<Value>(pCI));

								if (AOSNode *node = value_map[pV])
									handleAOSNode(node);
								else
									assert(false);
							}

							t++;
						}
					} else if (pF && (pF->getName() == "fscanf" ||
														pF->getName() == "__isoc99_fscanf" ||
														pF->getName() == "wscanf")) {
						//errs() << "Found " << pF->getName() << ": ";
						//U->dump();
						// Except arg 0 & 1
						int t = 0;
						for (auto arg = pCI->arg_begin(); arg != pCI->arg_end(); ++arg) {
							if (t != 0 && t != 1) {
								Value *pV = dyn_cast<Value>(arg);
								ex_input_set.insert(dyn_cast<Value>(pCI));

								if (AOSNode *node = value_map[pV])
									handleAOSNode(node);
								else
									assert(false);
							}

							t++;
						}
					} else if (pF && (pF->getName() == "fread" ||
														pF->getName() == "fread_unlocked" ||
														pF->getName() == "fgets" ||
														pF->getName() == "fgets_unlocked" ||
														pF->getName() == "gets" ||
														pF->getName() == "gets_unlocked" ||
														//pF->getName() == "read" ||
														pF->getName() == "getline")) {
						//errs() << "Found " << pF->getName() << ": ";
						//pCI->dump();
						// Arg 0
						int t = 0;
						for (auto arg = pCI->arg_begin(); arg != pCI->arg_end(); ++arg) {
							if (t == 0) {
								Value *pV = dyn_cast<Value>(arg);
								ex_input_set.insert(dyn_cast<Value>(pCI));

								if (AOSNode *node = value_map[pV])
									handleAOSNode(node);
								else {
                  pV->dump();
									assert(false);
                }
							}

							t++;
						}
					} else if (pF && (pF->getName() == "_ZNSi7getlineEPcl" ||
                            pF->getName() == "_ZSt7getlineIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RNSt7__cxx1112basic_stringIS4_S5_T1_EE" ||
                            pF->getName() == "_ZNSi7getlineEPclc" ||
														pF->getName() == "read")) {
						// std::iostream&.getline()
						//errs() << "Found " << pF->getName() << ": ";
						//U->dump();
						// Arg 1
						int t = 0;
						for (auto arg = pCI->arg_begin(); arg != pCI->arg_end(); ++arg) {
							if (t == 1) {
								Value *pV = dyn_cast<Value>(arg);
								ex_input_set.insert(dyn_cast<Value>(pCI));

								if (AOSNode *node = value_map[pV])
									handleAOSNode(node);
								else
									assert(false);
							}

							t++;
						}
					} else if (pF && (pF->getName() == "fgetc" ||
														pF->getName() == "fgetc_unlocked" ||
														pF->getName() == "getc" ||
														pF->getName() == "getc_unlocked" ||
														pF->getName() == "_IO_getc" ||
														pF->getName() == "getchar" ||
														pF->getName() == "getchar_unlocked" ||
														pF->getName() == "fgetwc" ||
														pF->getName() == "fgetwc_unlocked" ||
														pF->getName() == "getwc" ||
														pF->getName() == "getwc_unlocked" ||
														pF->getName() == "getwchar" ||
														pF->getName() == "getwchar_unlocked")) {
						//errs() << "Found " << pF->getName() << ": ";
						//pCI->dump();
						ex_input_set.insert(pCI);
						doTaintPropagation(pCI);
					}
				} else if (InvokeInst *pII = dyn_cast<InvokeInst>(&I)) {
					Function *pF = pII->getCalledFunction();

					if (pF && (pF->getName() == "_ZNSi7getlineEPcl" ||
                    pF->getName() == "_ZSt7getlineIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RNSt7__cxx1112basic_stringIS4_S5_T1_EE" ||
                    pF->getName() == "_ZNSi7getlineEPclc")) {
						// std::iostream&.getline()
						//errs() << "Found " << pF->getName() << ": ";
						//U->dump();
						// Arg 1
						int t = 0;
						for (auto arg = pII->arg_begin(); arg != pII->arg_end(); ++arg) {
							if (t == 1) {
								Value *pV = dyn_cast<Value>(arg);
								ex_input_set.insert(dyn_cast<Value>(pII));

								if (AOSNode *node = value_map[pV])
									handleAOSNode(node);
								else
									assert(false);
							}

							t++;
						}
					}
				}
			}
		}
	}
}

void AOSMarkTaintPass::handleAOSNode(AOSNode *node) {
  if (visit_node_set.find(node) != visit_node_set.end())
    return;

  visit_node_set.insert(node);
  //errs() << "Mark tainted\n";
  //printNode(node);
  node->setTainted(true);

  for (auto it = node->aliases.begin(); it != node->aliases.end(); it++) {
    Value *pV = it->first;

    for (auto pU: pV->users()) {
			if (auto pI = dyn_cast<Operator>(pU)) {
				switch (pI->getOpcode()) {
					//case Instruction::Invoke: TODO
					case Instruction::Call:
					{
						// if declare function, need to taint the return value?
						CallInst *pCI = dyn_cast<CallInst>(pI);
						Function *pF = pCI->getCalledFunction();

						if (pF && pF->isDeclaration())
							handleIntrinsicFunction(pF, pV, pCI);

						break;
					}
					case Instruction::Load:
					{
						auto pLI = dyn_cast<LoadInst>(pI);
						// Taint & propagate data loaded from tainted pointers
						// Mark nodes that store tainted data
						doTaintPropagation(pLI);

						break;
					}
				}
			}
    }
  }
}

void AOSMarkTaintPass::doTaintPropagation(Value *pV) {
  if (visit_val_set.find(pV) != visit_val_set.end())
    return;

  visit_val_set.insert(pV);

	for (auto pU: pV->users()) {
    //errs() << "pV->users(): "; pU->dump();
		if (auto pI = dyn_cast<Operator>(pU)) {
			switch (pI->getOpcode()) {
				case Instruction::Invoke:
				case Instruction::Call:
				{
					Value *arg = getArgument(pU, pV);

					if (arg)
            doTaintPropagation(arg);

          // if declare function, need to taint the return value?
          Function *pF;

          if (InvokeInst *pII = dyn_cast<InvokeInst>(pI))
            pF = pII->getCalledFunction();
          else if (CallInst *pCI = dyn_cast<CallInst>(pI))
            pF = pCI->getCalledFunction();

	        if (pF && pF->isDeclaration()) {
            //errs() << "pF->getName(): " << pF->getName() << "\n";
            //pI->dump();
            doTaintPropagation(pI);
          }

          break;
        }
				case Instruction::Load:
				{
					break;
				}
				case Instruction::Store:
				{
					auto pSI = dyn_cast<StoreInst>(pI);
          // Handle other node to taint

					if (pSI->getValueOperand() == pV) {
						auto ptrOp = pSI->getPointerOperand();

            if (AOSNode *node = value_map[ptrOp]) {
              handleAOSNode(node);
            } else {
              //pSI->dump();
              //assert(false);
            }
          }

          break;
        }
				case Instruction::Ret:
				{
					ReturnInst *pRI = dyn_cast<ReturnInst>(pI);

					if (pRI->getReturnValue() == pV) {
						for (auto pUb: pRI->getFunction()->users()) {
							if (Instruction *pIb = dyn_cast<Instruction>(pUb)) {
								Function *pF = pIb->getFunction();

								// To avoid the situation where CallInst doesn't use the return value
								if (pF && pIb->getType() == pRI->getReturnValue()->getType()) {
                  doTaintPropagation(pIb);
                }
              }
            }
          }
 
          break;
        }
				case Instruction::GetElementPtr:
				{
          if (AOSNode *node = value_map[pI])
            handleAOSNode(node);

					break;
				}
        default:
        {
          doTaintPropagation(pI);

          break;
        }
      }
    }
  }
}

Value *AOSMarkTaintPass::getArgument(Value *pI, Value *pV) {
	Function *pF;

	if (InvokeInst *pII = dyn_cast<InvokeInst>(pI))
		pF = pII->getCalledFunction();
	else if (CallInst *pCI = dyn_cast<CallInst>(pI))
		pF = pCI->getCalledFunction();
	else
		assert(false);

	//TODO can intrinsic func return pointer alias?
	if (pF && !pF->isVarArg() && !pF->isDeclaration()) {
		unsigned arg_nth = 0;

		if (InvokeInst *pII = dyn_cast<InvokeInst>(pI)) {
			for (auto arg = pII->arg_begin(); arg != pII->arg_end(); ++arg) {
				if (dyn_cast<Value>(arg) == pV)
					break;

				arg_nth++;
			}

			assert(arg_nth < pII->arg_size());
		}	else if (CallInst *pCI = dyn_cast<CallInst>(pI)) {
			for (auto arg = pCI->arg_begin(); arg != pCI->arg_end(); ++arg) {
				if (dyn_cast<Value>(arg) == pV)
					break;

				arg_nth++;
			}

			assert(arg_nth < pCI->arg_size());
		}

		assert(!pF->isVarArg()); //TODO: handle variable number of arguments

		unsigned t = 0;
		for (auto arg = pF->arg_begin(); arg != pF->arg_end(); arg++) {
			if (t++ == arg_nth)
				return arg;
		}
	}

	return nullptr;
}

void AOSMarkTaintPass::printNode(AOSNode *node) {
  errs() << "Print node!\n";
	for (auto it = node->aliases.begin(); it != node->aliases.end(); it ++) {
		it->first->dump();
	}
}

void AOSMarkTaintPass::handleIntrinsicFunction(Function *pF, Value *pV, CallInst *pCI) {

	if (pF->getName() == "memcpy" ||
			pF->getName() == "memmove" ||
			pF->getName() == "strcat" ||
			pF->getName() == "strbcat" ||
			pF->getName() == "strcpy" ||
			pF->getName() == "strncpy" ||
			pF->getName() == "strxfrm") {

		if (pCI->getOperand(1) == pV) {
			auto dest = pCI->getOperand(0);

			if (AOSNode *dest_node = value_map[dest])
				handleAOSNode(dest_node);

			if (pF->getName() == "strxfrm")
				doTaintPropagation(pCI);
		}
	} else if (pF->getName() == "memcmp" ||
							pF->getName() == "strcmp" ||
							pF->getName() == "strncmp" ||
							pF->getName() == "strcoll" ||
							pF->getName() == "strcspn" ||
							pF->getName() == "strlen" ||
							pF->getName() == "strspn") {
		doTaintPropagation(pCI);

	} else {
		doTaintPropagation(pCI);
	}

}

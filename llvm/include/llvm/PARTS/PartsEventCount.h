//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (C) 2018 Secure Systems Group, Aalto University <ssg.aalto.fi>
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_PARTSEVENTCOUNT_H
#define LLVM_PARTSEVENTCOUNT_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

namespace llvm {

namespace PARTS {

class PartsEventCount {

public:
  static Function *getFuncCodePointerBranch(Module &M) { return getCounterFunc(M, "__parts_count_code_ptr_branch"); };
  static Function *getFuncCodePointerCreate(Module &M) { return getCounterFunc(M, "__parts_count_code_ptr_create"); };
  static Function *getFuncDataStr(Module &M) { return getCounterFunc(M, "__parts_count_data_ptr_str"); };
  static Function *getFuncDataLdr(Module &M) { return getCounterFunc(M, "__parts_count_data_ptr_ldr"); };
  static Function *getFuncNonleafCall(Module &M) { return getCounterFunc(M, "__parts_count_nonleaf_call"); };
  static Function *getFuncLeafCall(Module &M) { return getCounterFunc(M, "__parts_count_leaf_call"); };

private:
  static Function *getCounterFunc(Module &M, const std::string &fName);
};

}

}

#endif //LLVM_PARTSEVENTCOUNT_H

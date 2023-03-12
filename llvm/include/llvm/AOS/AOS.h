#ifndef LLVM_AOS_H
#define LLVM_AOS_H

#include "llvm/IR/Constant.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"

namespace llvm {
namespace AOS {
enum AOSHeapInstType {
  AOSHeapInstDisabled,
  AOSHeapInstEnabled
};

enum AOSStackInstType {
  AOSStackInstDisabled,
  AOSStackInstEnabled
};

AOSHeapInstType getAOSHeapInstType();
AOSStackInstType getAOSStackInstType();

Pass *createAOSGlobalVariableOptPass(); //yh+
Pass *createAOSOptPass(); //yh+
Pass *createAOSMallocPass(); //yh+
Pass *createAOSOptDpiPass(); //yh+
Pass *createAOSInputDetectPass(); //yh+
Pass *createAOSBBCounterPass(); //yh+
Pass *createAOSPointerAliasPass(); //yh+
Pass *createAOSMarkTaintPass(); //yh+
Pass *createAOSReachTestPass(); //yh+
Pass *createAOSTaintOptPass(); //yh+
}
} // llvm

#endif //LLVM_AOS_H

#ifndef LLVM_WYFY_H
#define LLVM_WYFY_H

#include "llvm/IR/Constant.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"

namespace llvm {
namespace WYFY {

enum WyfyDpiType {
  WyfyNone,
  AOS,
  WYFY_C,
  WYFY_F,
  WYFY_FT
};

WyfyDpiType getWyfyDpiType();
bool isEnableFuncCounter();

Pass *createWyfyFuncCounterPass();
Pass *createWyfyPtrAliasPass();
Pass *createWyfyTaintPropPass();
Pass *createWyfyDataPtrSignPass();
}
} // llvm

#endif //LLVM_WYFY_H

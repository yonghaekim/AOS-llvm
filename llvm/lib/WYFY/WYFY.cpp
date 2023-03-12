#include "llvm/WYFY/WYFY.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include <regex>
#include <map>

using namespace llvm;
using namespace WYFY;

static cl::opt<bool> EnableFuncCounter("wyfy-count", cl::Hidden,
                                      cl::desc("WYFY function counter pass"),
                                      cl::init(false));

static cl::opt<WYFY::WyfyDpiType> WyfyDataPtrInst(
    "wyfy-dpi", cl::init(WyfyNone),
    cl::desc("WYFY data-pointer protection"),
    cl::value_desc("mode"),
    cl::values(clEnumValN(WyfyNone, "baseline", "Disable protection"),
               clEnumValN(AOS, "aos", "Enable AOS protection"),
               clEnumValN(WYFY_C, "wyfy-c", "Enable WYFY-C protection"),
               clEnumValN(WYFY_F, "wyfy-f", "Enable WYFY-F protection"),
               clEnumValN(WYFY_FT, "wyfy-ft", "Enable WYFY-FT protection")));

WyfyDpiType WYFY::getWyfyDpiType() {
  return WyfyDataPtrInst;
}

bool WYFY::isEnableFuncCounter() {
  return EnableFuncCounter;
}

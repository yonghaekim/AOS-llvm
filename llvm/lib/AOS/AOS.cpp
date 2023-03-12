#include "llvm/AOS/AOS.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include <regex>
#include <map>

using namespace llvm;
using namespace AOS;

static cl::opt<AOS::AOSHeapInstType> AOSHeapInst(
    "aos-heap", cl::init(AOSHeapInstDisabled),
    cl::desc("AOS heap protection"),
    cl::value_desc("mode"),
    cl::values(clEnumValN(AOSHeapInstDisabled, "disable", "Disable AOS heap protection"),
               clEnumValN(AOSHeapInstEnabled, "enable", "Enable AOS heap protection")));

static cl::opt<AOS::AOSStackInstType> AOSStackInst(
    "aos-stack", cl::init(AOSStackInstDisabled),
    cl::desc("AOS stack protection"),
    cl::value_desc("mode"),
    cl::values(clEnumValN(AOSStackInstDisabled, "disable", "Disable AOS stack protection"),
               clEnumValN(AOSStackInstEnabled, "enable", "Enable AOS stack protection")));

AOSHeapInstType AOS::getAOSHeapInstType() {
  return AOSHeapInst;
}

AOSStackInstType AOS::getAOSStackInstType() {
  return AOSStackInst;
}

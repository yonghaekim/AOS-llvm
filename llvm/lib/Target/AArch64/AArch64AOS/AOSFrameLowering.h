#ifndef LLVM_AOSFRAMELOWERING_H
#define LLVM_AOSFRAMELOWERING_H

#include "AArch64InstrInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"

namespace llvm {
namespace AOSFrameLowering {

void instrumentPrologue(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI,
                        MachineBasicBlock &MBB, MachineBasicBlock::iterator &MBBI,
                        const DebugLoc &DL);

void instrumentEpilogue(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI, MachineBasicBlock &MBB);

}
}

#endif //LLVM_AOSFRAMELOWERING_H

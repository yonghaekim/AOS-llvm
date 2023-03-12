#include "AOSFrameLowering.h"
#include "AArch64InstrInfo.h"
#include "AArch64MachineFunctionInfo.h"
#include "AArch64RegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <sstream>

using namespace llvm;

static bool doInstrument(const MachineFunction &MF) {
  for (const auto &Info : MF.getFrameInfo().getCalleeSavedInfo())
    if (Info.getReg() != AArch64::LR)
      return true;

  return false;
}

void AOSFrameLowering::instrumentPrologue(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI,
                                            MachineBasicBlock &MBB, MachineBasicBlock::iterator &MBBI,
                                            const DebugLoc &DL) {
  if (!doInstrument(*MBB.getParent()))
    return;

  auto *MF = MBB.getParent();
  const AArch64FunctionInfo *AFI = MF->getInfo<AArch64FunctionInfo>();

  assert(AFI->hasStackFrame() && "hasStackFrame() == false");

  auto *MI = &*MBBI; 
  signed StackOffset = -1 * ((signed) (AFI->getLocalStackSize()
                          + AFI->getCalleeSavedStackSize()));

  if (MI != nullptr) {
    // pacia sp, x16
    BuildMI(MBB, MI, MI->getDebugLoc(), TII->get(AArch64::PACIB), AArch64::SP)
        .addUse(AArch64::X16);
    // bndsl sp, #imm
    BuildMI(MBB, MI, MI->getDebugLoc(), TII->get(AArch64::AOS_BNDSL_IMM), AArch64::SP)
        .addImm(StackOffset);
    // bndsu sp, #imm
    BuildMI(MBB, MI, MI->getDebugLoc(), TII->get(AArch64::AOS_BNDSU_IMM), AArch64::SP)
        .addImm(StackOffset);
  } else {
    // pacia sp, x16
    BuildMI(&MBB, DebugLoc(), TII->get(AArch64::PACIB), AArch64::SP)
        .addUse(AArch64::X16);
    // bndsl sp, #imm
    BuildMI(&MBB, DebugLoc(), TII->get(AArch64::AOS_BNDSL_IMM), AArch64::SP)
        .addImm(StackOffset);
    // bndsu sp, #imm
    BuildMI(&MBB, DebugLoc(), TII->get(AArch64::AOS_BNDSU_IMM), AArch64::SP)
        .addImm(StackOffset);
  }
}

void AOSFrameLowering::instrumentEpilogue(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI,
                                  MachineBasicBlock &MBB) {
  if (!doInstrument(*MBB.getParent()))
    return;

  const auto loc = MBB.getFirstTerminator();
  auto *MI = (loc != MBB.end() ? &*loc : nullptr);

  if (MI != nullptr) {
    // bndrm sp
    BuildMI(MBB, MI, MI->getDebugLoc(), TII->get(AArch64::AOS_BNDRM), AArch64::SP);
  } else {
    // bndrm sp
    BuildMI(&MBB, DebugLoc(), TII->get(AArch64::AOS_BNDRM), AArch64::SP);
  }
}



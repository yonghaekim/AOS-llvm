#include "AOSFrameLowering.h"
#include "AArch64InstrInfo.h"
#include "AArch64MachineFunctionInfo.h"
#include "AArch64RegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/AOS/AOS.h"
#include <sstream>

using namespace llvm;
using namespace llvm::AOS;

static bool doInstrument(const MachineFunction &MF) {
  //static const auto AOSStackInst = AOS::getAOSStackInstType();

  //if (AOSStackInst == AOSStackInstDisabled) return false;

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
 
  unsigned StackOffset = AFI->getLocalStackSize() + AFI->getCalleeSavedStackSize() - 16;

  // pacma fp, sp
  //BuildMI(MBB, MBBI, DL, TII->get(AArch64::AOS_PACMA), AArch64::SP)
  //    .addUse(AArch64::SP)
  //    .addReg(AArch64::Z31);
  //BuildMI(MBB, MBBI, DL, TII->get(AArch64::AOS_CHECK), AArch64::Z31);
  BuildMI(MBB, MBBI, DL, TII->get(AArch64::WYFY_BNDSTR), AArch64::SP)
      .addUse(AArch64::Z31);
  //BuildMI(MBB, MBBI, DL, TII->get(AArch64::AOS_CHECK), AArch64::Z31);
  //BuildMI(MBB, MBBI, DL, TII->get(AArch64::AOS_CHECK), AArch64::Z31);

}

void AOSFrameLowering::instrumentEpilogue(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI,
                                  MachineBasicBlock &MBB) {
  if (!doInstrument(*MBB.getParent()))
    return;

  const auto loc = MBB.getFirstTerminator();
  auto *MI = (loc != MBB.end() ? &*loc : nullptr);

  if (!MI)
    return;

  auto MIi = MBB.begin();
  for (auto MIie = MBB.end(); MIi != MIie; MIi++)
    if (MIi->getFlag(MachineInstr::FrameDestroy))
      break;

  BuildMI(MBB, MIi, MIi->getDebugLoc(), TII->get(AArch64::WYFY_BNDCLR), AArch64::SP);
  //BuildMI(MBB, MIi, MIi->getDebugLoc(), TII->get(AArch64::AOS_XPACM), AArch64::SP);
  //BuildMI(MBB, MIi, MIi->getDebugLoc(), TII->get(AArch64::AOS_CHECK), AArch64::Z31);
  //BuildMI(MBB, MIi, MIi->getDebugLoc(), TII->get(AArch64::AOS_CHECK), AArch64::Z31);
  //BuildMI(MBB, MIi, MIi->getDebugLoc(), TII->get(AArch64::AOS_CHECK), AArch64::Z31);
}



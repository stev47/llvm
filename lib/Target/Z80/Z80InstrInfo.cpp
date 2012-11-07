//===-- Z80InstrInfo.cpp - Z80 Instruction Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Z80InstrInfo.h"
#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define GET_INSTRINFO_CTOR
#include "Z80GenInstrInfo.inc"

using namespace llvm;

Z80InstrInfo::Z80InstrInfo(Z80TargetMachine &tm)
  : Z80GenInstrInfo(Z80::ADJCALLSTACKDOWN, Z80::ADJCALLSTACKUP),
  RI(tm, *this), TM(tm)
{
}

void Z80InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator I, DebugLoc dl,
  unsigned DestReg, unsigned SrcReg, bool KillSrc) const
{
  if (Z80::GR8RegClass.contains(DestReg, SrcReg))
  {
    BuildMI(MBB, I, dl, get(Z80::LD8rr), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  }
  else if (Z80::GR16RegClass.contains(DestReg, SrcReg))
  {
    if (SrcReg != Z80::IX && DestReg != Z80::IX &&
      SrcReg != Z80::IY && DestReg != Z80::IY)
    {
      unsigned SrcSubReg, DestSubReg;

      SrcSubReg  = RI.getSubReg(SrcReg,  Z80::sub_8bit_low);
      DestSubReg = RI.getSubReg(DestReg, Z80::sub_8bit_low);
      BuildMI(MBB, I, dl, get(Z80::LD8rr), DestSubReg)
        .addReg(SrcSubReg, getKillRegState(KillSrc));

      SrcSubReg  = RI.getSubReg(SrcReg,  Z80::sub_8bit_hi);
      DestSubReg = RI.getSubReg(DestReg, Z80::sub_8bit_hi);
      BuildMI(MBB, I, dl, get(Z80::LD8rr), DestSubReg)
        .addReg(SrcSubReg, getKillRegState(KillSrc));
    }
    else
    {
      BuildMI(MBB, I, dl, get(Z80::PUSH16r))
        .addReg(SrcReg, getKillRegState(KillSrc));
      BuildMI(MBB, I, dl, get(Z80::POP16r), DestReg);
      return;
    }
  }
  else
    llvm_unreachable("Imossible reg-to-reg copy");
}

void Z80InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  unsigned SrcReg, bool isKill, int FrameIndex,
  const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();

  MachineMemOperand *MMO = MF.getMachineMemOperand(
    MachinePointerInfo::getFixedStack(FrameIndex),
    MachineMemOperand::MOStore,
    MFI.getObjectSize(FrameIndex),
    MFI.getObjectAlignment(FrameIndex));

  unsigned Opc;

  if (RC == &Z80::GR8RegClass)
    Opc = Z80::LD8mr;
  else
    llvm_unreachable("Cannot store this register to stack slot!");

  BuildMI(MBB, MI, dl, get(Opc))
    .addFrameIndex(FrameIndex).addImm(0)
    .addReg(SrcReg, getKillRegState(isKill)).addMemOperand(MMO);
  MF.dump();
}

void Z80InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  unsigned DestReg,  int FrameIndex,
  const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();

  MachineMemOperand *MMO = MF.getMachineMemOperand(
    MachinePointerInfo::getFixedStack(FrameIndex),
    MachineMemOperand::MOLoad,
    MFI.getObjectSize(FrameIndex),
    MFI.getObjectAlignment(FrameIndex));

  unsigned Opc;

  if (RC == &Z80::GR8RegClass)
    Opc = Z80::LD8rm;
  else
    llvm_unreachable("Cannot load this register from stack slot!");

  BuildMI(MBB, MI, dl, get(Opc))
    .addReg(DestReg)
    .addFrameIndex(FrameIndex).addImm(0).addMemOperand(MMO);
  MF.dump();
}

bool Z80InstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
  SmallVectorImpl<MachineOperand> &Cond, bool AllowModify = false) const
{
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analysis.
    if (!I->isBranch())
      return true;

    // Handle uncoditional branches.
    if (I->getOpcode() == Z80::JP)
    {
      if (!AllowModify)
      {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JP, delete them.
      while (llvm::next(I) != MBB.end())
        llvm::next(I)->eraseFromParent();

      Cond.clear();
      FBB = 0;

      // Delete the JP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB()))
      {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }
      // TBB is used to indicate the unconditional destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    // Handle conditional branches.
    assert(I->getOpcode() == Z80::JPCC && "Invalid conditional branch");
    Z80::CondCode BranchCC = static_cast<Z80::CondCode>(I->getOperand(1).getImm());
    if (BranchCC == Z80::COND_INVALID)
      return true;

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty())
    {
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCC));
      continue;
    }
    // Handle subsequent conditional branches.
    assert(0 && "Not implemented yet!");
  }

  return false;
}

unsigned Z80InstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;
    if (I->getOpcode() != Z80::JP &&
        I->getOpcode() != Z80::JPCC)
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.end();
    Count++;
  }
  return Count;
}

unsigned Z80InstrInfo::InsertBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *TBB, MachineBasicBlock *FBB,
  const SmallVectorImpl<MachineOperand> &Cond,
  DebugLoc DL) const
{
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 0 || Cond.size() == 1) &&
    "Z80 branch conditions have one component!");

  if (Cond.empty())
  {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(Z80::JP)).addMBB(TBB);
    return 1;
  }

  // Conditional branch.
  unsigned Count = 0;
  BuildMI(&MBB, DL, get(Z80::JPCC)).addMBB(TBB).addImm(Cond[0].getImm());
  Count++;

  if (FBB)
  {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(Z80::JP)).addMBB(FBB);
    Count++;
  }
  return Count;
}

//===-- Z80RegisterInfo.cpp - Z80 Register Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "Z80RegisterInfo.h"
#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/ADT/BitVector.h"

#define GET_REGINFO_TARGET_DESC
#include "Z80GenRegisterInfo.inc"

using namespace llvm;

Z80RegisterInfo::Z80RegisterInfo(Z80TargetMachine &tm, const TargetInstrInfo &tii)
	: Z80GenRegisterInfo(Z80::PC), TM(tm), TII(tii)
{
}

const uint16_t* Z80RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
	static const uint16_t CalleeSavedRegs[] = {
		0
	};
	return CalleeSavedRegs;
}

BitVector Z80RegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
	BitVector Reserved(getNumRegs());

	Reserved.set(Z80::PC);
	Reserved.set(Z80::SP);
	Reserved.set(Z80::FLAGS);

	return Reserved;
}

void Z80RegisterInfo::eliminateCallFramePseudoInstr(MachineFunction &MF,
	MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const
{
	const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
	bool reserveCallFrame = TFI->hasReservedCallFrame(MF);
	int Opcode = I->getOpcode();
	bool isDestroy = Opcode == TII.getCallFrameDestroyOpcode();
	uint64_t CalleAmt = isDestroy ? I->getOperand(1).getImm() : 0;

	if (!reserveCallFrame)
	{
		assert(0 && "Not Implemented yet!");
	}

	if (isDestroy && CalleAmt)
	{
			assert(0 && "Not Implemented yet!");
	}
	MBB.erase(I);
}

void Z80RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator I,
	int SPAdj, RegScavenger *RS) const
{
	assert(SPAdj == 0 && "Unexpected");

	unsigned i = 0;
	MachineInstr &MI = *I;
	MachineBasicBlock &MBB = *MI.getParent();
	MachineFunction &MF = *MBB.getParent();
	const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
	DebugLoc dl = MI.getDebugLoc();

	while (!MI.getOperand(i).isFI())
	{
		i++;
		assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
	}

	int FrameIndex = MI.getOperand(i).getIndex();
	int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
	Offset += MF.getFrameInfo()->getStackSize();

	if (!TFI->hasFP(MF))
	{
		Offset += 2;
		MI.getOperand(i).ChangeToRegister(Z80::IX, false);
		MI.getOperand(i+1).ChangeToImmediate(Offset);
	}
	else assert(0 && "Not Implemented yet!");
}

unsigned Z80RegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
	assert(0 && "Not Implemented yet!");
	return 0;
}

//===-- Z80FrameLowering.cpp - Z80 Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Z80FrameLowering.h"
#include "Z80.h"
#include "Z80InstrInfo.h"
#include "Z80MachineFunctionInfo.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
using namespace llvm;

Z80FrameLowering::Z80FrameLowering(const Z80TargetMachine &tm)
	: TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 2, -2),
	TM(tm)
{
}

bool Z80FrameLowering::hasFP(const MachineFunction &MF) const
{
	const MachineFrameInfo *MFI = MF.getFrameInfo();
	return (MF.getTarget().Options.DisableFramePointerElim(MF) ||
		MFI->hasVarSizedObjects() ||
		MFI->isFrameAddressTaken());
}

void Z80FrameLowering::emitPrologue(MachineFunction &MF) const
{
	MachineBasicBlock &MBB = MF.front();
	MachineFrameInfo *MFI = MF.getFrameInfo();
	Z80MachineFunctionInfo *Z80MFI = MF.getInfo<Z80MachineFunctionInfo>();
	const Z80InstrInfo &TII = *TM.getInstrInfo();
	MachineBasicBlock::iterator MBBI = MBB.begin();
	DebugLoc DL = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();

	// Get the number of bytes to allocate from the FrameInfo.
	uint64_t StackSize = MFI->getStackSize();

	uint64_t NumBytes = 0;
	if (hasFP(MF))
	{
		assert(0 && "Not Implemented yet!");
	}
	else NumBytes = StackSize;

	// Skip the callee-saved push instructions
	while (MBBI != MBB.end() && (MBBI->getOpcode() == Z80::PUSH16r))
		MBBI++;

	if (MBBI != MBB.end())
		DL = MBBI->getDebugLoc();

	if (NumBytes)
	{
		BuildMI(MBB, MBBI, DL, TII.get(Z80::PUSH16r), Z80::IX);
		BuildMI(MBB, MBBI, DL, TII.get(Z80::LD16ri), Z80::IX).addImm(-NumBytes);
		BuildMI(MBB, MBBI, DL, TII.get(Z80::ADD16rr), Z80::IX).addReg(Z80::IX).addReg(Z80::SP);
		BuildMI(MBB, MBBI, DL, TII.get(Z80::LD16rr), Z80::SP).addReg(Z80::IX);
	}
}

void Z80FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
	const MachineFrameInfo *MFI = MF.getFrameInfo();
	Z80MachineFunctionInfo *Z80MFI = MF.getInfo<Z80MachineFunctionInfo>();
	const Z80InstrInfo &TII = *TM.getInstrInfo();
	MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
	DebugLoc DL = MBBI->getDebugLoc();

	unsigned RetOpcode = MBBI->getOpcode();
	switch (RetOpcode)
	{
	default:
		llvm_unreachable("Can only insert epilog into returning blocks");
	case Z80::RET:
		break;
	}
	uint64_t StackSize = MFI->getStackSize();
	uint64_t NumBytes = 0;

	if (hasFP(MF))
	{
		assert(0 && "Not Implemented yet!");
	}
	else NumBytes = StackSize;

	// Skip the callee-saved pop instructions
	while (MBBI != MBB.begin())
	{
		MachineBasicBlock::iterator PI = prior(MBBI);
		unsigned Opc = PI->getOpcode();
		if (Opc != Z80::POP16r && !PI->isTerminator())
			break;
		MBBI--;
	}

	DL = MBBI->getDebugLoc();

	if (MFI->hasVarSizedObjects())
	{
		assert(0 && "Not Implemented yet!");
	}
	else
	{	// adjust stack pointer back: SP += numbytes
		if (NumBytes)
		{
			BuildMI(MBB, MBBI, DL, TII.get(Z80::LD16ri), Z80::IX).addImm(NumBytes);
			BuildMI(MBB, MBBI, DL, TII.get(Z80::ADD16rr), Z80::IX).addReg(Z80::IX).addReg(Z80::SP);
			BuildMI(MBB, MBBI, DL, TII.get(Z80::LD16rr), Z80::SP).addReg(Z80::IX);
			BuildMI(MBB, MBBI, DL, TII.get(Z80::POP16r), Z80::IX);
		}
	}
}

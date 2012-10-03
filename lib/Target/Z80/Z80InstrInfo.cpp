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
	unsigned Opc;
	if (Z80::GR8RegClass.contains(DestReg, SrcReg))
		Opc = Z80::LD8rr;
	else
		llvm_unreachable("Imossible reg-to-reg copy");

	BuildMI(MBB, I, dl, get(Opc), DestReg)
		.addReg(SrcReg, getKillRegState(KillSrc));
}

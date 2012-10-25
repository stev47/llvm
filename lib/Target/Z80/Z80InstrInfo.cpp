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
	unsigned Opc;
	if (Z80::GR8RegClass.contains(DestReg, SrcReg))
		Opc = Z80::LD8rr;
	else
		llvm_unreachable("Imossible reg-to-reg copy");

	BuildMI(MBB, I, dl, get(Opc), DestReg)
		.addReg(SrcReg, getKillRegState(KillSrc));
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

	if (RC == &Z80::ACCRegClass)
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

	if (RC == &Z80::ACCRegClass)
		Opc = Z80::LD8rm;
	else
		llvm_unreachable("Cannot store this register to stack slot!");

	BuildMI(MBB, MI, dl, get(Opc))
		.addReg(DestReg)
		.addFrameIndex(FrameIndex).addImm(0).addMemOperand(MMO);
	MF.dump();
}

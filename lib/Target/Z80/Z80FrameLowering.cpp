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
using namespace llvm;

Z80FrameLowering::Z80FrameLowering()
	: TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 2, -2)
{
}

bool Z80FrameLowering::hasFP(const MachineFunction &MF) const
{
	assert(0 && "Not Implemented yet!");
	return true;
}

void Z80FrameLowering::emitPrologue(MachineFunction &MF) const
{
}

void Z80FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
}

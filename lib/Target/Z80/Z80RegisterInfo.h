//===-- Z80RegisterInfo.h - Z80 Register Information Impl -------*- C++ -*-===//
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

#ifndef LLVM_TARGET_Z80REGISTERINFO_H
#define LLVM_TARGET_Z80REGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "Z80GenRegisterInfo.inc"

namespace llvm {

	class TargetInstrInfo;
	class Z80TargetMachine;

	class Z80RegisterInfo : public Z80GenRegisterInfo {
	private:
		Z80TargetMachine &TM;
		const TargetInstrInfo &TII;
	public:
		Z80RegisterInfo(Z80TargetMachine &tm, const TargetInstrInfo &tii);
	}; // end class Z80RegisterInfo
}; // end namespace llvm

#endif

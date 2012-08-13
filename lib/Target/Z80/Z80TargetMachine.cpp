//===-- Z80TargetMachine.cpp - Define TargetMachine for the Z80 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Z80 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeZ80Target() {
    RegisterTargetMachine<Z80TargetMachine> X(TheZ80Target);
}

Z80TargetMachine::Z80TargetMachine(const Target &T,
        StringRef TT,
        StringRef CPU,
        StringRef FS,
		const TargetOptions &Options,
        Reloc::Model RM, CodeModel::Model CM,
		CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
	Subtarget(TT, CPU, FS),
	DataLayout("e-p:16:16:16-i8:8:8-i16:16:16-i32:16:32"),
	InstrInfo(*this),
	FrameLowering()
{ }

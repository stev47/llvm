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

#include "Z80TargetMachine.h"
#include "Z80.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
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
	InstrInfo(*this), TLInfo(*this), TSInfo(*this),
	FrameLowering()
{ }

namespace {
	class Z80PassConfig : public TargetPassConfig {
	public:
		Z80PassConfig(Z80TargetMachine *TM, PassManagerBase &PM)
			: TargetPassConfig(TM, PM)
		{}
		Z80TargetMachine &getZ80TargetMachine() const {
			return getTM<Z80TargetMachine>();
		}
		virtual bool addInstSelector();
		virtual bool addPreEmitPass();
	}; // end class Z80PassConfig
} // end namespace

TargetPassConfig *Z80TargetMachine::createPassConfig(PassManagerBase &PM)
{
	return new Z80PassConfig(this, PM);
}

bool Z80PassConfig::addInstSelector()
{
	PM->add(createZ80ISelDag(getZ80TargetMachine(), getOptLevel()));
	return false;
}

bool Z80PassConfig::addPreEmitPass()
{
	assert(0 && "Not Implemented yet!");
	return false;
}

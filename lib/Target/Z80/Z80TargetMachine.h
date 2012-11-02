//==-- Z80TargetMachine.h - Define TargetMachine for Z80 ---------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Z80 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef Z80TARGETMACHINE_H
#define Z80TARGETMACHINE_H

#include "Z80.h"
#include "Z80ISelLowering.h"
#include "Z80InstrInfo.h"
#include "Z80FrameLowering.h"
#include "Z80SelectionDAGInfo.h"
#include "Z80Subtarget.h"
#include "llvm/Target/TargetData.h"

namespace llvm {

  class Z80TargetMachine : public LLVMTargetMachine {
    Z80Subtarget     Subtarget;
    const TargetData DataLayout;	// Calculates type size && alignment
    Z80InstrInfo     InstrInfo;
    Z80TargetLowering TLInfo;
    Z80SelectionDAGInfo	TSInfo;
    Z80FrameLowering FrameLowering;
  public:
    Z80TargetMachine(const Target &T, StringRef TT,
      StringRef CPU, StringRef FS, const TargetOptions &Options,
      Reloc::Model RM, CodeModel::Model CM,
      CodeGenOpt::Level OL);
    virtual const TargetFrameLowering *getFrameLowering() const { return &FrameLowering; }
    virtual const Z80TargetLowering *getTargetLowering() const { return &TLInfo; }
    virtual const Z80InstrInfo *getInstrInfo() const { return &InstrInfo; }
    virtual const TargetData *getTargetData() const { return &DataLayout; }
    virtual const Z80Subtarget *getSubtargetImpl() const { return &Subtarget; }
    virtual const Z80RegisterInfo *getRegisterInfo() const {
      return &getInstrInfo()->getRegisterInfo();
    }
    virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
  }; // Z80TargetMachine
} // end namespace llvm

#endif

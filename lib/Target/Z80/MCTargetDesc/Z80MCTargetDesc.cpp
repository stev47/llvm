//===-- Z80MCTargetDesc.cpp - Z80 Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Z80 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Z80MCTargetDesc.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_MC_DESC
#include "Z80GenSubtargetInfo.inc"

using namespace llvm;

static MCSubtargetInfo *createZ80MCSubtargetInfo(StringRef TT,
                                                 StringRef CPU,
                                                 StringRef FS) {
    MCSubtargetInfo *X = new MCSubtargetInfo();
    InitZ80MCSubtargetInfo(X, TT, CPU, FS);
    return X;
}

extern "C" void LLVMInitializeZ80TargetMC() {
    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(TheZ80Target,
                                            createZ80MCSubtargetInfo);
}

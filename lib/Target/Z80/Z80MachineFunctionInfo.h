//===-- Z80MachineFuctionInfo.h - Z80 machine function info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Z80-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef Z80MACHINEFUNCTIONINFO_H
#define Z80MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
  class Z80MachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();
  public:
    Z80MachineFunctionInfo() {}
    explicit Z80MachineFunctionInfo(MachineFunction &MF) {}
  }; // end class Z80MachineFunctionInfo
} // end namespace llvm

#endif

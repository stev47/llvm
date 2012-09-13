//===-- Z80InstrInfo.td - Z80 Instruction defs -------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Z80 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions.
//===----------------------------------------------------------------------===//

include "Z80InstrFormats.td"

//===----------------------------------------------------------------------===//
// Z80 profiles and nodes
//===----------------------------------------------------------------------===//

def Z80Ret : SDNode<"Z80ISD::RET", SDTNone, [SDNPHasChain, SDNPOptInGlue]>;

//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//

let isReturn = 1, isTerminator = 1 in {
  def RET : Z80Inst<(outs), (ins), "ret", [(Z80Ret)]>;
}

def NOP : Z80Inst<(outs), (ins), "nop", []>;

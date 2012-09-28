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

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET : Z80Inst<(outs), (ins), "ret", [(Z80Ret)]>;
}

let neverHasSideEffects = 1 in
def NOP : Z80Inst<(outs), (ins), "nop", []>;

//===----------------------------------------------------------------------===//
//  Load Instructions.
//
let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def LD8ri : Z80Inst<(outs GR8:$dst), (ins i8imm:$src),
                      "ld\t{$dst, $src}",
                      [(set GR8:$dst, imm:$src)]>;
  def LD16ri : Z80Inst<(outs GR16:$dst), (ins i16imm:$src),
                       "ld\t{$dst, $src}",
                       [(set GR16:$dst, imm:$src)]>;
}
def LD16rr : Z80Inst<(outs GR16:$dst), (ins GR16:$src),
                     "ld\t{$dst, $src}",
                     []>;

//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions.
//
let Defs = [SP], Uses = [SP], neverHasSideEffects = 1 in {
let mayLoad = 1 in
def POP16r : Z80Inst<(outs GR16:$reg), (ins),
                     "pop\t$reg", []>;

let mayStore = 1 in
def PUSH16r : Z80Inst<(outs), (ins GR16:$reg),
                      "push\t$reg", []>;
}

//===----------------------------------------------------------------------===//
// Arithmetic Instructions
//
let Constraints = "$src = $dst" in {
def ADD16rr : Z80Inst<(outs GR16:$dst), (ins GR16:$src, GR16:$src2),
                      "add\t{$dst, $src2}",
                      []>;
} // Constraints = "$src = $dst"

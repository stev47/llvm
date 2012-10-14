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
// Z80 Type Profiles.
//===----------------------------------------------------------------------===//
def SDT_Z80Call         : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_Z80CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_Z80CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

//===----------------------------------------------------------------------===//
// Z80 Specific Node Definitions.
//===----------------------------------------------------------------------===//
def Z80call          : SDNode<"Z80ISD::CALL", SDT_Z80Call,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def Z80callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_Z80CallSeqStart,
                     [SDNPHasChain, SDNPOutGlue]>;
def Z80callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_Z80CallSeqEnd,
                     [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def Z80ret           : SDNode<"Z80ISD::RET", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue]>;

//===----------------------------------------------------------------------===//
// Z80 Operand Definitions.
//===----------------------------------------------------------------------===//

// Address operands
def mem : Operand<i16> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GR16, i8imm);
}

//===----------------------------------------------------------------------===//
// Z80 Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;
//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//

let Defs = [SP, FLAGS], Uses = [SP] in {
  def ADJCALLSTACKDOWN : Z80Inst<(outs), (ins i16imm:$amt),
                                 "#ADJCALLSTACKDOWN",
                                 [(Z80callseq_start timm:$amt)]>;
  def ADJCALLSTACKUP   : Z80Inst<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                 "#ADJCALLSTACKUP",
                                 [(Z80callseq_end timm:$amt1, timm:$amt2)]>;
}

let neverHasSideEffects = 1 in
def NOP : Z80Inst<(outs), (ins), "nop", []>;
//===----------------------------------------------------------------------===//
//  Call Instructions.
//
let isCall = 1, Defs = [HL, DE, BC], Uses = [SP] in {
  def CALLi : Z80Inst<(outs), (ins i16imm:$dst, variable_ops),
                      "call\t$dst", [(Z80call imm:$dst)]>;
}

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET : Z80Inst<(outs), (ins), "ret", [(Z80ret)]>;
}
//===----------------------------------------------------------------------===//
//  Load Instructions.
//
let neverHasSideEffects = 1 in {
  def LD8rr : Z80Inst<(outs GR8:$dst), (ins GR8:$src),
                      "ld\t{$dst, $src}",
                      []>;
  def LD16rr : Z80Inst<(outs GR16:$dst), (ins GR16:$src),
                       "ld\t{$dst, $src}",
                       []>;
}

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def LD8ri : Z80Inst<(outs GR8:$dst), (ins i8imm:$src),
                      "ld\t{$dst, $src}",
                      [(set GR8:$dst, imm:$src)]>;
  def LD16ri : Z80Inst<(outs GR16:$dst), (ins i16imm:$src),
                       "ld\t{$dst, $src}",
                       [(set GR16:$dst, imm:$src)]>;
}

let canFoldAsLoad = 1, isReMaterializable = 1 in {
def LD8rm : Z80Inst<(outs GR8:$dst), (ins mem:$src),
                    "ld\t{$dst, $src}",
                    [(set GR8:$dst, (load addr:$src))]>;
def LD16rm : Z80Inst<(outs GR16:$dst), (ins mem:$src),
                     "ld\t{${dst:sub_8bit_low}, $src}\n\tld\t{${dst:sub_8bit_hi}, ${src:1}}",
                     [(set GR16:$dst, (load addr:$src))]>;
}

def LD8mi : Z80Inst<(outs), (ins mem:$dst, i8imm:$src),
                    "ld\t{$dst, $src}",
                    [(store (i8 imm:$src), addr:$dst)]>;
def LD8mr : Z80Inst<(outs), (ins mem:$dst, GR8:$src),
                    "ld\t{$dst}, $src}",
                    [(store GR8:$src, addr:$dst)]>;
def LD16mi : Z80Inst<(outs), (ins mem:$dst, i16imm:$src),
                     "ld\t{$dst, ${src:8bit_low}}\n\tld\t{${dst:1}, ${src:8bit_hi}}",
                     [(store (i16 imm:$src), addr:$dst)]>;
def LD16mr : Z80Inst<(outs), (ins mem:$dst, GR16:$src),
                     "ld\t{$dst, ${src:sub_8bit_low}}\n\tld\t{${dst:1}, ${src:sub_8bit_hi}}",
                     [(store GR16:$src, addr:$dst)]>;

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
let Constraints = "$src = $dst", Defs = [FLAGS] in {
def ADD16rr : Z80Inst<(outs GR16:$dst), (ins GR16:$src, GR16:$src2),
                      "add\t{$dst, $src2}",
                      []>;
def ADD16HLr : Z80Inst<(outs GR16HL:$dst), (ins GR16:$src, GR16:$src2),
                       "add\t{$dst, $src2}",
                       [(set GR16HL:$dst, (add GR16:$src, GR16:$src2)),
                       (implicit FLAGS)]>;
def ADD8Ar : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, GR8:$src2),
                     "add\t{$dst, $src2}",
                     [(set GR8A:$dst, (add GR8:$src, GR8:$src2)),
                     (implicit FLAGS)]>;
def ADD8Ai : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, i8imm:$src2),
                     "add\t{$dst, $src2}",
                     [(set GR8A:$dst, (add GR8:$src, imm:$src2)),
                     (implicit FLAGS)]>;
def ADD8Am : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, mem:$src2),
                     "add\t{$dst, $src2}",
                     [(set GR8A:$dst, (add GR8:$src, (load addr:$src2))),
                     (implicit FLAGS)]>;
let Uses = [FLAGS] in {
def ADC16HLr : Z80Inst<(outs GR16HL:$dst), (ins GR16:$src, GR16:$src2),
                       "adc\t{$dst, $src2}",
                       [(set GR16HL:$dst, (adde GR16:$src, GR16:$src2)),
                       (implicit FLAGS)]>;
def ADC8Ar : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, GR8:$src2),
                     "adc\t{$dst, $src2}",
                     [(set GR8A:$dst, (adde GR8:$src, GR8:$src2)),
                     (implicit FLAGS)]>;
def ADC8Ai : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, i8imm:$src2),
                     "adc\t{$dst, $src2}",
                     [(set GR8A:$dst, (adde GR8:$src, imm:$src2)),
                     (implicit FLAGS)]>;
def ADC8Am : Z80Inst<(outs GR8A:$dst), (ins GR8:$src, mem:$src2),
                     "adc\t{$dst, $src2}",
                     [(set GR8A:$dst, (adde GR8:$src, (load addr:$src2))),
                     (implicit FLAGS)]>;
} // Uses = [FLAGS]
} // Constraints = "$src = $dst"

//===----------------------------------------------------------------------===//
//  Non-Instruction Patterns.
//

// call
def : Pat<(Z80call (i16 tglobaladdr:$dst)),
          (CALLi tglobaladdr:$dst)>;

// add always produce carry
def : Pat<(addc GR16:$src, GR16:$src2),
          (ADD16HLr GR16:$src, GR16:$src2)>;
def : Pat<(addc GR8:$src, GR8:$src2),
          (ADD8Ar GR8:$src, GR8:$src2)>;
def : Pat<(addc GR8:$src, imm:$src2),
          (ADD8Ai GR8:$src, imm:$src2)>;
def : Pat<(addc GR8:$src, (load addr:$src2)),
          (ADD8Am GR8:$src, addr:$src2)>;

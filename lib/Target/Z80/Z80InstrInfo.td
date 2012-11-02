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
def SDT_Z80Wrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                                               SDTCisPtrTy<0>]>;
def SDT_Z80Cp           : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def SDT_Z80SelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>,
                                               SDTCisSameAs<1, 2>,
                                               SDTCisVT<3, i8>]>;
def SDT_Z80BrCC         : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>,
                                               SDTCisVT<1, i8>]>;
//===----------------------------------------------------------------------===//
// Z80 Specific Node Definitions.
//===----------------------------------------------------------------------===//
def Z80wrapper       : SDNode<"Z80ISD::WRAPPER", SDT_Z80Wrapper>;
def Z80call          : SDNode<"Z80ISD::CALL", SDT_Z80Call,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def Z80callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_Z80CallSeqStart,
                     [SDNPHasChain, SDNPOutGlue]>;
def Z80callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_Z80CallSeqEnd,
                     [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def Z80ret           : SDNode<"Z80ISD::RET", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue]>;
def Z80cp            : SDNode<"Z80ISD::CP", SDT_Z80Cp, [SDNPOutGlue]>;
def Z80selectcc      : SDNode<"Z80ISD::SELECT_CC", SDT_Z80SelectCC,
                     [SDNPInGlue]>;
def Z80brcc          : SDNode<"Z80ISD::BR_CC", SDT_Z80BrCC,
                     [SDNPHasChain, SDNPInGlue]>;
def Z80rlc           : SDNode<"Z80ISD::RLC", SDTIntUnaryOp, []>;
def Z80rrc           : SDNode<"Z80ISD::RRC", SDTIntUnaryOp, []>;
def Z80rl            : SDNode<"Z80ISD::RL",  SDTIntUnaryOp, []>;
def Z80rr            : SDNode<"Z80ISD::RR",  SDTIntUnaryOp, []>;
def Z80sla           : SDNode<"Z80ISD::SLA", SDTIntUnaryOp, []>;
def Z80sra           : SDNode<"Z80ISD::SRA", SDTIntUnaryOp, []>;
def Z80sll           : SDNode<"Z80ISD::SLL", SDTIntUnaryOp, []>;
def Z80srl           : SDNode<"Z80ISD::SRL", SDTIntUnaryOp, []>;
//===----------------------------------------------------------------------===//
// Z80 Operand Definitions.
//===----------------------------------------------------------------------===//

// Address operands
def mem : Operand<i16> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GR16, i8imm);
}

def jmptarget : Operand<OtherVT> {
  let PrintMethod = "printPCRelImmOperand";
}

def cc : Operand<i8> {
  let PrintMethod = "printCCOperand";
}

//===----------------------------------------------------------------------===//
// Z80 Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;
//===----------------------------------------------------------------------===//
// Pattern fragments.
//===----------------------------------------------------------------------===//

def Z80_COND_NZ : PatLeaf<(i8 0)>;
def Z80_COND_Z  : PatLeaf<(i8 1)>;
def Z80_COND_NC : PatLeaf<(i8 2)>;
def Z80_COND_C  : PatLeaf<(i8 3)>;
def Z80_COND_PO : PatLeaf<(i8 4)>;
def Z80_COND_PE : PatLeaf<(i8 5)>;
def Z80_COND_P  : PatLeaf<(i8 6)>;
def Z80_COND_M  : PatLeaf<(i8 7)>;

//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//
let Defs = [SP, FLAGS], Uses = [SP] in {
  def ADJCALLSTACKDOWN : PseudoI<(outs), (ins i16imm:$amt),
    [(Z80callseq_start timm:$amt)]>;

  def ADJCALLSTACKUP   : PseudoI<(outs), (ins i16imm:$amt1, i16imm:$amt2),
    [(Z80callseq_end timm:$amt1, timm:$amt2)]>;
}

let usesCustomInserter = 1 in {
  def SELECT8 : PseudoI<(outs GR8:$dst), (ins GR8:$src, GR8:$src2, i8imm:$cc),
    [(set GR8:$dst, (Z80selectcc GR8:$src, GR8:$src2, imm:$cc))]>;
}

let neverHasSideEffects = 1 in
def NOP : I<0x00, FRaw, (outs), (ins), "nop", []>;
//===----------------------------------------------------------------------===//
//  Call and Control Flow Instructions.
//===----------------------------------------------------------------------===//
let isCall = 1, Defs = [HL, DE, BC], Uses = [SP] in {
  def CALLi : Ii16<0xCD, FRaw, (outs), (ins i16imm:$dst, variable_ops),
    "call\t{$dst}", [(Z80call imm:$dst)]>;
}

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET : I<0xC9, FRaw, (outs), (ins), "ret", [(Z80ret)]>;
}

let isBranch = 1, isTerminator = 1 in {
  let isBarrier = 1 in
  def JP : Ii16<0xC3, FRaw, (outs), (ins jmptarget:$dst),
    "jp\t{$dst}", [(br bb:$dst)]>;
  let Uses = [FLAGS] in
  def JPCC : Ii16<0xCA, FRaw, (outs), (ins jmptarget:$dst, cc:$cc),
    "jp\t{$cc, $dst}", [(Z80brcc bb:$dst, imm:$cc)]>;
}
//===----------------------------------------------------------------------===//
//  Load Instructions.
//===----------------------------------------------------------------------===//
let neverHasSideEffects = 1 in {
  def LD8rr : I<0x40, FRR, (outs GR8:$dst), (ins GR8:$src),
    "ld\t{$dst, $src}", []>;
  def LD16rr : I<0xF9, FRaw, (outs GR16:$dst), (ins GR16:$src),
    "ld\t{$dst, $src}", []>;
}

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def LD8ri : Ii8<0x06, FRy, (outs GR8:$dst), (ins i8imm:$src),
    "ld\t{$dst, $src}",
    [(set GR8:$dst, imm:$src)]>;
  def LD16ri : Ii16<0x01, FRP, (outs GR16:$dst), (ins i16imm:$src),
    "ld\t{$dst, $src}",
    [(set GR16:$dst, imm:$src)]>;
}

let canFoldAsLoad = 1, isReMaterializable = 1 in {
  let Defs = [A] in
  def LD8Am : Ii16<0x3A, FRaw, (outs), (ins i16imm:$src),
    "ld\t{a, ($src)}",
    [(set A, (load imm:$src))]>;
  def LD8rm : I<0x40, FRy, (outs GR8:$dst), (ins mem:$src),
    "ld\t{$dst, $src}",
    [(set GR8:$dst, (load addr:$src))]>;
}

let Uses = [A] in
def LD8mA : Ii16<0x32, FRaw, (outs), (ins i16imm:$dst),
  "ld\t{($dst), a}",
  [(store A, imm:$dst)]>;
def LD8mr : I<0x40, FRz, (outs), (ins mem:$dst, GR8:$src),
  "ld\t{$dst, $src}",
  [(store GR8:$src, addr:$dst)]>;

//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions.
//===----------------------------------------------------------------------===//
let Defs = [SP], Uses = [SP], neverHasSideEffects = 1 in {
  let mayLoad = 1 in
    def POP16r  : I<0xC1, FRP, (outs GR16:$reg), (ins), "pop\t{$reg}", []>;

  let mayStore = 1 in
    def PUSH16r : I<0xC5, FRP, (outs), (ins GR16:$reg), "push\t{$reg}", []>;
}
//===----------------------------------------------------------------------===//
// Arithmetic Instructions
//===----------------------------------------------------------------------===//
let Defs = [FLAGS] in {
  def ADD16rr : I<0x09, FRP, (outs GR16:$dst), (ins GR16:$src, GR16:$src2),
    "add\t{$dst, $src2}", []>;
}
defm ADD8 : ALUI<ALU_ADD, "add\t{a, $src}">;
defm SUB8 : ALUI<ALU_SUB, "sub\t{$src}">;
defm AND8 : ALUI<ALU_AND, "and\t{$src}">;
defm XOR8 : ALUI<ALU_XOR, "xor\t{$src}">;
defm OR8  : ALUI<ALU_OR,  "or\t{$src}">;
defm CP8  : ALUI<ALU_CP,  "cp\t{$src}">;
let Uses = [A, FLAGS] in {
  defm ADC8 : ALUI<ALU_ADC, "adc\t{a, $src}">;
  defm SBC8 : ALUI<ALU_SBC, "sbc\t{a, $src}">;
}
defm RLC8 : RSI<RS_RLC, "rlc\t{$src}">;
defm RRC8 : RSI<RS_RRC, "rrc\t{$src}">;
defm SLA8 : RSI<RS_SLA, "sla\t{$src}">;
defm SRA8 : RSI<RS_SRA, "sra\t{$src}">;
defm SLL8 : RSI<RS_SLL, "sll\t{$src}">;
defm SRL8 : RSI<RS_SRL, "srl\t{$src}">;
let Uses = [FLAGS] in {
  defm RL8  : RSI<RS_RL,  "rl\t{$src}">;
  defm RR8  : RSI<RS_RR,  "rr\t{$src}">;
}
let Constraints = "$src = $dst" in {
let Defs = [FLAGS] in {
  def INC8r : I<0x04, FRy, (outs GR8:$dst), (ins GR8:$src),
    "inc\t{$src}", [(set GR8:$dst, (add GR8:$src, 1)), (implicit FLAGS)]>;
  def DEC8r : I<0x05, FRy, (outs GR8:$dst), (ins GR8:$src),
    "dec\t{$src}", [(set GR8:$dst, (add GR8:$src, -1)), (implicit FLAGS)]>;
} // Defs = [FLAGS]
  def INC16r : I<0x03, FRP, (outs GR16:$dst), (ins GR16:$src),
    "inc\t{$src}", [(set GR16:$dst, (add GR16:$src, 1))]>;
  def DEC16r : I<0x0B, FRP, (outs GR16:$dst), (ins GR16:$src),
    "dec\t{$src}", [(set GR16:$dst, (add GR16:$src, -1))]>;
}
let Defs = [A, FLAGS], Uses = [A] in {
  def CPL : I<0x2F, FRaw, (outs), (ins),
    "cpl", [(set A, (xor A, -1)), (implicit FLAGS)]>;
  def NEG : I<0x44, FRaw, (outs), (ins),
    "neg", [(set A, (add (xor A, -1), 1)), (implicit FLAGS)]>;
}
//===----------------------------------------------------------------------===//
//  Non-Instruction Patterns.
//===----------------------------------------------------------------------===//

// call
def : Pat<(Z80call (i16 tglobaladdr:$dst)), (CALLi tglobaladdr:$dst)>;
def : Pat<(Z80call (i16 texternalsym:$dst)), (CALLi texternalsym:$dst)>;

// add and sub always produce carry
def : Pat<(addc A, GR8:$src), (ADD8r GR8:$src)>;
def : Pat<(addc A, imm:$src), (ADD8i imm:$src)>;

def : Pat<(subc A, GR8:$src), (SUB8r GR8:$src)>;
def : Pat<(subc A, imm:$src), (SUB8i imm:$src)>;

// GlobalAddress
def : Pat<(load (i16 (Z80wrapper tglobaladdr:$src))), (LD8Am tglobaladdr:$src)>;
def : Pat<(store A, (i16 (Z80wrapper tglobaladdr:$dst))), (LD8mA tglobaladdr:$dst)>;

def : Pat<(i16 (Z80wrapper tglobaladdr:$dst)), (LD16ri tglobaladdr:$dst)>;

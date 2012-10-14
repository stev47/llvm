//===- Z80RegisterInfo.td - Describe the Z80 Register Info ----*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the Z80 register file
//===----------------------------------------------------------------------===//

let Namespace = "Z80" in {

  // Subregister indices
  def sub_8bit_hi  : SubRegIndex;
  def sub_8bit_low : SubRegIndex;

  // 8 bit registers
  def A : Register<"a">;
  def B : Register<"b">;
  def C : Register<"c">;
  def D : Register<"d">;
  def E : Register<"e">;
  def H : Register<"h">;
  def L : Register<"l">;
  def LX : Register<"lx">;
  def HX : Register<"hx">;
  def LY : Register<"ly">;
  def HY : Register<"hy">;

  // Flags register
  def FLAGS : Register<"f">;

  // 16 bit registers
  let SubRegIndices = [sub_8bit_hi, sub_8bit_low], CoveredBySubRegs = 1 in {
  def BC : RegisterWithSubRegs<"bc", [B, C]>;
  def DE : RegisterWithSubRegs<"de", [D, E]>;
  def HL : RegisterWithSubRegs<"hl", [H, L]>;
  def IX : RegisterWithSubRegs<"ix", [HX, LX]>;
  def IY : RegisterWithSubRegs<"iy", [HY, LY]>;
  }
  def SP : Register<"sp">;
  def PC : Register<"pc">;
  
  def GR8 : RegisterClass<"Z80", [i8], 8, (add A, B, C, D, E, H, L, HX, LX, HY, LY, FLAGS)>;
  def GR16 : RegisterClass<"Z80", [i16], 8, (add BC, DE, HL, IX, IY, PC, SP)> {
    let SubRegClasses = [(GR8 sub_8bit_hi, sub_8bit_low)];
  }
  def GR8A : RegisterClass<"Z80", [i8], 8, (add A)>;
  def GR16HL : RegisterClass<"Z80", [i16], 8, (add HL)>;
}

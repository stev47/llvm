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
  def A : Register<"A">;
  def B : Register<"B">;
  def C : Register<"C">;
  def D : Register<"D">;
  def E : Register<"E">;
  def H : Register<"H">;
  def L : Register<"L">;

  // 16 bit registers
  let SubRegIndices = [sub_8bit_hi, sub_8bit_low] in {
  def BC : RegisterWithSubRegs<"BC", [B, C]>;
  def DE : RegisterWithSubRegs<"DE", [D, E]>;
  def HL : RegisterWithSubRegs<"HL", [H, L]>;
  }
  def SP : Register<"SP">;
  def PC : Register<"PC">;
  
  def GR8 : RegisterClass<"Z80", [i8], 8, (add A, B, C, D, E, H, L)>;
  def GR16 : RegisterClass<"Z80", [i16], 8, (add BC, DE, HL, PC, SP)>;
}

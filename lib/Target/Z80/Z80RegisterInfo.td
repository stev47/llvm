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

class Z80Reg<string n, bits<3> num = 0>
  : Register<n> {
  field bits<3> Num = num;
  let Namespace = "Z80";
}

class Z80SubRegIndex : SubRegIndex {
  let Namespace = "Z80";
}

def sub_8bit_hi  : Z80SubRegIndex;
def sub_8bit_low : Z80SubRegIndex;

class Z80RegWithSubRegs<string n, list<Register> subregs, bits<2> num = 0>
  : RegisterWithSubRegs<n, subregs> {
  field bits<2> Num = num;
  let SubRegIndices = [sub_8bit_hi, sub_8bit_low];
  let CoveredBySubRegs = 1;
  let Namespace = "Z80";
}

class Z80Reg8Class<dag reglist>
  : RegisterClass<"Z80", [i8], 8, reglist> {
}

class Z80Reg16Class<dag reglist>
  : RegisterClass<"Z80", [i16], 8, reglist> {
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

// 8 bit registers
def B : Z80Reg<"b", 0>;
def C : Z80Reg<"c", 1>;
def D : Z80Reg<"d", 2>;
def E : Z80Reg<"e", 3>;
def H : Z80Reg<"h", 4>;
def L : Z80Reg<"l", 5>;
def A : Z80Reg<"a", 7>;
def HX : Z80Reg<"hx", 4>;
def LX : Z80Reg<"lx", 5>;
def HY : Z80Reg<"hy", 4>;
def LY : Z80Reg<"ly", 5>;

// Flags register
def FLAGS : Z80Reg<"f">;

// Interrupt register
def I : Z80Reg<"i">;

// Memory refresh register
def R : Z80Reg<"r">;

// 16 bit registers
def BC : Z80RegWithSubRegs<"bc", [B, C], 0>;
def DE : Z80RegWithSubRegs<"de", [D, E], 1>;
def HL : Z80RegWithSubRegs<"hl", [H, L], 2>;
def AF : Z80RegWithSubRegs<"af", [A, FLAGS], 3>;
def IX : Z80RegWithSubRegs<"ix", [HX, LX], 2>;
def IY : Z80RegWithSubRegs<"iy", [HY, LY], 2>;
def SP : Z80Reg<"sp", 3>;
def PC : Z80Reg<"pc">;

def GR8  : RegisterClass<"Z80", [i8],  8,
  (add A, B, C, D, E, H, L, I, R, HX, LX, HY, LY, FLAGS)>;
def GR16 : RegisterClass<"Z80", [i16], 8,
  (add BC, DE, HL, IX, IY, PC, SP)> {
  let SubRegClasses = [(GR8 sub_8bit_hi, sub_8bit_low)];
}

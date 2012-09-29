//===-- Z80CallingConv.td - Z80 Calling Conventions --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Z80 architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_Z80 : CallingConv<[
  // i8 are returned in registers A
  CCIfType<[i8], CCAssignToReg<[A]>>,

  // i16 are returned in registers HL
  CCIfType<[i16], CCAssignToReg<[HL]>>
]>;

//===----------------------------------------------------------------------===//
// Z80 Argument Calling Conventions
//===----------------------------------------------------------------------===//

def CC_Z80 : CallingConv<[
  CCIfNotVarArg<CCIfType<[i8], CCAssignToReg<[A]>>>,

  // The first 3 integer arguments of non-varargs functions are passed in
  // integer registers
  CCIfNotVarArg<CCIfType<[i16], CCAssignToReg<[HL, DE, BC]>>>,

  // Integer values get stored in stack slots that are 2 bytes in
  // size and 2-byte aligned
  CCIfType<[i16], CCAssignToStack<2, 2>>
]>;

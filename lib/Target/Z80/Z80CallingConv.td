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
]>;

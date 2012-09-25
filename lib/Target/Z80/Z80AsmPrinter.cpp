//===-- Z80AsmPrinter.cpp - Z80 LLVM Assembly Printer ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to Z80 machine code.
//
//===----------------------------------------------------------------------===//

#include "Z80AsmPrinter.h"
#include "Z80.h"
#include "Z80MCInstLower.h"
#include "InstPrinter/Z80InstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

void Z80AsmPrinter::EmitInstruction(const MachineInstr *MI)
{
	Z80MCInstLower MCInstLowering(Mang, *MF, *this);

	MCInst TmpInst;
	MCInstLowering.Lower(MI, TmpInst);
	OutStreamer.EmitInstruction(TmpInst);
}

void Z80AsmPrinter::printOperand(const MachineInstr *MI, unsigned OpNum,
	raw_ostream &O, const char *Modifier)
{
	const MachineOperand &MO = MI->getOperand(OpNum);
	switch (MO.getType())
	{
	case MachineOperand::MO_Register:
		O << Z80InstPrinter::getRegisterName(MO.getReg());
		return;
	case MachineOperand::MO_Immediate:
		O << MO.getImm();
		return;
	default:
		llvm_unreachable("Not implemented yet!");
	}
}

//===----------------------------------------------------------------------===//
// Target Registry Stuff
//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeZ80AsmPrinter()
{
	RegisterAsmPrinter<Z80AsmPrinter> X(TheZ80Target);
}

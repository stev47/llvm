//===-- Z80InstPrinter.cpp - Convert Z80 MCInst to assembly syntax --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Z80 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "Z80InstPrinter.h"
#include "Z80.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// Include the auto-generated portion of the assembler writter
#include "Z80GenAsmWriter.inc"

void Z80InstPrinter::printInst(const MCInst *MI, raw_ostream &O,
	StringRef Annot)
{
	printInstruction(MI, O);
	printAnnotation(O, Annot);
}

void Z80InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
	raw_ostream &O, const char *Modifier)
{
	assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
	const MCOperand &Op = MI->getOperand(OpNo);
	if (Op.isReg())
	{
		O << getRegisterName(Op.getReg());
	}
	else if (Op.isImm())
	{
		O << Op.getImm();
	}
	else
	{
		assert(Op.isExpr() && "unknown operand kind in printOperand");
		O << *Op.getExpr();
	}
}

void Z80InstPrinter::printMemOperand(const MCInst *MI, unsigned OpNo,
	raw_ostream &O, const char *Modifier)
{
	const MCOperand &Base = MI->getOperand(OpNo);
	const MCOperand &Disp = MI->getOperand(OpNo+1);

	assert(Disp.isImm() && "Expected immediate in displacement field");
	if (Base.getReg())
		O << '(' << getRegisterName(Base.getReg()) << '+' << Disp.getImm()<< ')';
}

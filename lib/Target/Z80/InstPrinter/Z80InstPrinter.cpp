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
#include "llvm/MC/MCRegisterInfo.h"
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
	const MCOperand &Op = MI->getOperand(OpNo);
	if (Op.isReg())
	{
		unsigned Reg = Op.getReg();
		if (Modifier)
		{
			StringRef mod(Modifier);
			if (mod == "sub_8bit_low")
				Reg = MRI.getSubReg(Reg, Z80::sub_8bit_low);
			else if (mod == "sub_8bit_hi")
				Reg = MRI.getSubReg(Reg, Z80::sub_8bit_hi);
			else
				llvm_unreachable("Invalid Register Modifier");
		}
		O << getRegisterName(Reg);
	}
	else if (Op.isImm())
	{
		int64_t Value = Op.getImm();
		if (Modifier)
		{
			StringRef mod(Modifier);
			if (mod == "8bit_low")
				Value &= 0xFF;
			else if (mod == "8bit_hi")
				Value = (Value>>8) & 0xFF;
			else
				llvm_unreachable("Invalid Immediate Modifier");
		}
		O << Value;
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

	unsigned Idx = Disp.getImm();
	if (Modifier)
	{
		unsigned Offset;
		StringRef(Modifier).getAsInteger(0, Offset);
		Idx += Offset;
	}

	if (Base.getReg())
	{
		if (Idx >= 0)	O << '(' << getRegisterName(Base.getReg()) << '+' << Idx<< ')';
		else O << '(' << getRegisterName(Base.getReg()) << Idx << ')';
	}
	else llvm_unreachable("Invalid operand");
}

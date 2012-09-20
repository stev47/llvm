//===-- Z80AsmPrinter.h - Z80 LLVM Assembly Printer -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Z80 assembly code printer class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80ASMPRINTER_H
#define Z80ASMPRINTER_H

#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {
	class Z80AsmPrinter : public AsmPrinter {
		const Z80Subtarget *Subtarget;
	public:
		explicit Z80AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
			: AsmPrinter(TM, Streamer) {
				Subtarget = &TM.getSubtarget<Z80Subtarget>();
		}
		virtual const char *getPassName() const { return "Z80 Assembly Printer"; }

		void EmitInstruction(const MachineInstr *MI);

		// tablegen function declaration
		void printInstruction(const MachineInstr *MI, raw_ostream &OS);
		static const char* getRegisterName(unsigned RegNo);
	}; // end class Z80AsmPrinter
} // end namespace llvm

#endif

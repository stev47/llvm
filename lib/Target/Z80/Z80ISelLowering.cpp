//===-- Z80ISelLowering.cpp - X86 DAG Lowering Implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Z80 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Z80ISelLowering.h"
#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
using namespace llvm;

Z80TargetLowering::Z80TargetLowering(Z80TargetMachine &TM)
	: TargetLowering(TM, new TargetLoweringObjectFileELF())
{
	addRegisterClass(MVT::i8, Z80::GR8RegisterClass);
	addRegisterClass(MVT::i16, Z80::GR16RegisterClass);

	computeRegisterProperties();
}
//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "Z80GenCallingConv.inc"

SDValue Z80TargetLowering::LowerFormalArguments(SDValue Chain,
										CallingConv::ID CallConv, bool isVarArg,
										const SmallVectorImpl<ISD::InputArg> &Ins,
										DebugLoc dl, SelectionDAG &DAG,
										SmallVectorImpl<SDValue> &InVals) const
{
	return Chain;
}

SDValue Z80TargetLowering::LowerReturn(SDValue Chain,
	CallingConv::ID CallConv, bool isVarArg,
	const SmallVectorImpl<ISD::OutputArg> &Outs,
	const SmallVectorImpl<SDValue> &OutVals,
	DebugLoc dl, SelectionDAG &DAG) const
{
	SmallVector<CCValAssign, 16> RVLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		getTargetMachine(), RVLocs, *DAG.getContext());
	CCInfo.AnalyzeReturn(Outs, RetCC_Z80);

	// Add the regs to the liveout set for the function
	if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
		for (unsigned i = 0; i!= RVLocs.size(); i++)
			if (RVLocs[i].isRegLoc())
				DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
	}

	SDValue Flag;

	// Copy the result values into the output registers
	for (unsigned i = 0; i != RVLocs.size(); i++)
	{
		CCValAssign &VA = RVLocs[i];
		assert(VA.isRegLoc() && "Can only return in registers!");

		Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

		// Guarantee that all emitted copies are stuck together,
		// avoiding something bad
		Flag = Chain.getValue(1);
	}
	if (Flag.getNode())
		return DAG.getNode(Z80ISD::RET, dl, MVT::Other, Chain, Flag);
	return DAG.getNode(Z80ISD::RET, dl, MVT::Other, Chain);
}

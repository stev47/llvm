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
	addRegisterClass(MVT::i8, Z80::GR8ARegisterClass);

	computeRegisterProperties();

	setStackPointerRegisterToSaveRestore(Z80::SP);
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
	MachineFunction &MF = DAG.getMachineFunction();
	MachineRegisterInfo &MRI = MF.getRegInfo();
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		getTargetMachine(), ArgLocs, *DAG.getContext());
	CCInfo.AnalyzeFormalArguments(Ins, CC_Z80);

	assert(!isVarArg && "Varargs not supported yet");

	for (unsigned i = 0, e = ArgLocs.size(); i != e; i++)
	{
		SDValue ArgValue;
		unsigned VReg;

		CCValAssign &VA = ArgLocs[i];
		if (VA.isRegLoc())
		{	// Argument passed in registers
			EVT RegVT = VA.getLocVT();
			switch (RegVT.getSimpleVT().SimpleTy)
			{
			default:
				llvm_unreachable("unknown argument type!");
			case MVT::i16:
				VReg = MRI.createVirtualRegister(Z80::GR16RegisterClass);
				MRI.addLiveIn(VA.getLocReg(), VReg);
				ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
				InVals.push_back(ArgValue);
				break;
			case MVT::i8:
				VReg = MRI.createVirtualRegister(Z80::GR8RegisterClass);
				MRI.addLiveIn(VA.getLocReg(), VReg);
				ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
				InVals.push_back(ArgValue);
				break;
			}
		}
		else
		{
			assert(0 && "Not Implemented yet!");
		}
	}
	return Chain;
}

SDValue Z80TargetLowering::LowerCall(SDValue Chain, SDValue Callee,
	CallingConv::ID CallConv, bool isVarArg,
	bool doesNotRet, bool &isTailCall,
	const SmallVectorImpl<ISD::OutputArg> &Outs,
	const SmallVectorImpl<SDValue> &OutVals,
	const SmallVectorImpl<ISD::InputArg> &Ins,
	DebugLoc dl, SelectionDAG &DAG,
	SmallVectorImpl<SDValue> &InVals) const
{
	if (isTailCall) llvm_unreachable("Unsupported tail call optimization");
	
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		getTargetMachine(), ArgLocs, *DAG.getContext());
	CCInfo.AnalyzeCallOperands(Outs, CC_Z80);

	// Get a count of how many bytes are to be pushed on the stack
	unsigned NumBytes = CCInfo.getNextStackOffset();

	Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true));

	SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
	SmallVector<SDValue, 12> MemOpChains;

	// Walk the register/memloc assignments, inserting copies/loads
	for (unsigned i = 0, e = ArgLocs.size(); i != e; i++)
	{
		assert(0 && "Not Implemented yet!");
	}
	SDValue InFlag;

	if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
		Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i16);
	else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
		Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);

	// Returns a chain & a flag for retval copy to use
	SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
	SmallVector<SDValue, 8> Ops;
	Ops.push_back(Chain);
	Ops.push_back(Callee);

	Chain = DAG.getNode(Z80ISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
	InFlag = Chain.getValue(1);

	// Create the CALLSEQ_END node
	Chain = DAG.getCALLSEQ_END(Chain,
		DAG.getIntPtrConstant(NumBytes, true),
		DAG.getIntPtrConstant(0, true),
		InFlag);
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

const char *Z80TargetLowering::getTargetNodeName(unsigned Opcode) const
{
	switch (Opcode)
	{
	default: return NULL;
	case Z80ISD::CALL: return "Z80ISD::CALL";
	case Z80ISD::RET:  return "Z80ISD::RET";
	}
}

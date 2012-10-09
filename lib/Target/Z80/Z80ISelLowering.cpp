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

	setOperationAction(ISD::STORE, MVT::i16, Custom);

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
	// Z80 target does not yet support tail call optimization
	isTailCall = false;
	
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		getTargetMachine(), ArgLocs, *DAG.getContext());
	CCInfo.AnalyzeCallOperands(Outs, CC_Z80);

	// Get a count of how many bytes are to be pushed on the stack
	unsigned NumBytes = CCInfo.getNextStackOffset();

	Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true));

	SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
	SmallVector<SDValue, 12> MemOpChains;
	SDValue StackPtr;

	// Walk the register/memloc assignments, inserting copies/loads
	for (unsigned i = 0, e = ArgLocs.size(); i != e; i++)
	{
		CCValAssign &VA = ArgLocs[i];
		SDValue Arg = OutVals[i];

		switch (VA.getLocInfo())
		{
		default: llvm_unreachable("Unknown loc info!");
		case CCValAssign::Full: break;
		case CCValAssign::AExt:
			Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
			break;
		}

		// Arguments that can be passed on register must be kept at RegsToPass
		// vector
		if (VA.isRegLoc()) {
			RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
		}
		else
		{
			assert(VA.isMemLoc());

			if (StackPtr.getNode() == 0)
				StackPtr = DAG.getCopyFromReg(Chain, dl, Z80::IX, getPointerTy());

			SDValue PtrOff = DAG.getNode(ISD::ADD, dl, getPointerTy(), StackPtr,
				DAG.getIntPtrConstant(VA.getLocMemOffset()));
			MemOpChains.push_back(DAG.getStore(Chain, dl, Arg, PtrOff,
				MachinePointerInfo(), false, false, 0));
		}
	}

	// Transform all store nodes into one single node because all store nodes are
	// independent of each other.
	if (!MemOpChains.empty()) {
		Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
			&MemOpChains[0], MemOpChains.size());
	}

	// Build a sequence of copy-to-reg nodes chained together
	// with token chain and flag operands which copy the outgoing
	// args into registers. The InFlag in necessary since all
	// emitted insturctions must be stuck together.
	SDValue InFlag;
	for (unsigned i = 0, e = RegsToPass.size(); i != e; i++)
	{
		Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first, RegsToPass[i].second, InFlag);
		InFlag = Chain.getValue(1);
	}

	if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
		Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i16);
	else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
		Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);

	// Returns a chain & a flag for retval copy to use
	SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
	SmallVector<SDValue, 8> Ops;
	Ops.push_back(Chain);
	Ops.push_back(Callee);

	// Add argument registers to the end of the list so that they are
	// known live into the call.
	for (unsigned i = 0, e = RegsToPass.size(); i != e; i++)
		Ops.push_back(DAG.getRegister(RegsToPass[i].first, RegsToPass[i].second.getValueType()));

	if (InFlag.getNode())
		Ops.push_back(InFlag);

	Chain = DAG.getNode(Z80ISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
	InFlag = Chain.getValue(1);

	// Create the CALLSEQ_END node
	Chain = DAG.getCALLSEQ_END(Chain,
		DAG.getIntPtrConstant(NumBytes, true),
		DAG.getIntPtrConstant(0, true),
		InFlag);
	InFlag = Chain.getValue(1);

	return LowerCallResult(Chain, InFlag, CallConv, isVarArg,
		Ins, dl,DAG, InVals);
}

SDValue Z80TargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
	CallingConv::ID CallConv, bool isVarArg,
	const SmallVectorImpl<ISD::InputArg> &Ins,
	DebugLoc dl, SelectionDAG &DAG,
	SmallVectorImpl<SDValue> &InVals) const
{
	SmallVector<CCValAssign, 16> RVLocs;
	CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		getTargetMachine(), RVLocs, *DAG.getContext());
	CCInfo.AnalyzeCallResult(Ins, RetCC_Z80);

	// Copy all of the result registers out of their specified physreg
	for (unsigned i = 0; i != RVLocs.size(); i++)
	{
		Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
			RVLocs[i].getValVT(), InFlag).getValue(1);
		InFlag = Chain.getValue(2);
		InVals.push_back(Chain.getValue(0));
	}
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

SDValue Z80TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
	switch (Op.getOpcode()) {
	case ISD::STORE:	return LowerStore(Op, DAG);
	default:
		llvm_unreachable("unimplemented operand");
	}
}

SDValue Z80TargetLowering::LowerStore(SDValue Op, SelectionDAG &DAG) const
{
	StoreSDNode *ST = cast<StoreSDNode>(Op);
	assert(!ST->isTruncatingStore() && "Unexpected store type");
	assert(ST->getMemoryVT() == MVT::i16 && "Unexpected store EVT");
	if (allowsUnalignedMemoryAccesses(ST->getMemoryVT()))
		return SDValue();
	SDValue Chain = ST->getChain();
	SDValue BasePtr = ST->getBasePtr();
	//assert(BasePtr.getOpcode() == ISD::FrameIndex && "LowerStore support only FrameIndex Pointer");
	DebugLoc dl = ST->getDebugLoc();
	// Split Value to type i8
	SDValue Lo = DAG.getConstant(ST->getConstantOperandVal(1) & 0xFF, MVT::i8);
	SDValue Hi = DAG.getConstant((ST->getConstantOperandVal(1)>>8) & 0xFF, MVT::i8);

	// Separate store High and Low parts of i16
	SDValue StoreLow = DAG.getStore(Chain, dl, Lo, BasePtr,
		ST->getPointerInfo(),
		ST->isVolatile(),
		ST->isNonTemporal(),
		ST->getAlignment());
	SDValue HighAddr = DAG.getNode(ISD::ADD, dl, MVT::i16, BasePtr,
		DAG.getConstant(1, MVT::i16));
	SDValue StoreHigh = DAG.getStore(Chain, dl, Hi, HighAddr,
		ST->getPointerInfo(),
		ST->isVolatile(),
		ST->isNonTemporal(),
		ST->getAlignment());
	return DAG.getNode(ISD::TokenFactor, dl, MVT::Other, StoreLow, StoreHigh);
}

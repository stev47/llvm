//===- Z80ISelDAGToDAG.cpp - A DAG pattern matching inst selector for Z80 -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a DAG pattern matching instruction selector for Z80,
// converting from a legalized dag to a Z80 dag.
//
//===----------------------------------------------------------------------===//

#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {
	class Z80DAGToDAGISel : public SelectionDAGISel {
		const Z80TargetLowering &Lowering;
		const Z80Subtarget &Subtarget;
	public:
		Z80DAGToDAGISel(Z80TargetMachine &TM, CodeGenOpt::Level OptLevel)
			: SelectionDAGISel(TM, OptLevel),
			Lowering(*TM.getTargetLowering()),
			Subtarget(*TM.getSubtargetImpl())
		{ }
	#include "Z80GenDAGISel.inc"
	private:
		SDNode *Select(SDNode *N);
		bool SelectAddr(SDValue N, SDValue &Base, SDValue &Disp);
	}; // end class Z80DAGToDAGISel
} // end namespace

FunctionPass *llvm::createZ80ISelDag(Z80TargetMachine &TM, CodeGenOpt::Level OptLevel)
{
	return new Z80DAGToDAGISel(TM, OptLevel);
}

bool Z80DAGToDAGISel::SelectAddr(SDValue N, SDValue &Base, SDValue &Disp)
{
	switch (N->getOpcode())
	{
	default:
		Base = N;
		Disp = CurDAG->getTargetConstant(0, MVT::i8);
		return true;
	case ISD::TargetGlobalAddress:
		return false;
	case ISD::ADD:
	case ISD::OR:
		if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N.getOperand(1)))
		{
			if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N.getOperand(0)))
			{
				Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
			}
			else
			{
				Base = N.getOperand(0);
			}
			Disp = CurDAG->getTargetConstant(CN->getZExtValue(), MVT::i8);
		}
		return true;
	case ISD::FrameIndex:
		if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N))
		{
			Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
			Disp = CurDAG->getTargetConstant(0, MVT::i8);
		}
		return true;
	}
}

SDNode *Z80DAGToDAGISel::Select(SDNode *Node)
{
	DebugLoc dl = Node->getDebugLoc();
	
	// Dump information about the Node being selected
	DEBUG(errs() << "Selecting: ");
	DEBUG(Node->dump(CurDAG));
	DEBUG(errs() << "\n");

	// If we have a custom node, we already have selected
	if (Node->isMachineOpcode()) {
		DEBUG(errs() << "== ";
		Node->dump(CurDAG);
		errs() << "\n");
		return NULL;
	}

	switch (Node->getOpcode())
	{
	default: break;
	}

	// Select the default instruction
	SDNode *ResNode = SelectCode(Node);

	DEBUG(errs() << "=> ");
	if (ResNode == NULL || ResNode == Node)
		DEBUG(Node->dump(CurDAG));
	else
		DEBUG(ResNode->dump(CurDAG));
	DEBUG(errs() << "\n");

	return ResNode;
}

//===-- Z80ISelLowering.h - X86 DAG Lowering Interface ----------*- C++ -*-===//
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

#ifndef Z80ISELLOWERING_H
#define Z80ISELLOWERING_H

#include "Z80RegisterInfo.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
	namespace Z80ISD {
		enum NodeType {
			FIRST_NUMBER = ISD::BUILTIN_OP_END,
			CALL,
			RET
		};
	} // end namespace Z80ISD

	class Z80TargetLowering: public TargetLowering {
	public:
		explicit Z80TargetLowering(Z80TargetMachine &TM);
		// getTargetNodeName - This method returns the name of a target specific
		// DAG node.
		virtual const char *getTargetNodeName(unsigned Opcode) const;
	private:
		SDValue
			LowerCallResult(SDValue Chain, SDValue InFlag,
				CallingConv::ID CallConv, bool isVarArg,
				const SmallVectorImpl<ISD::InputArg> &Ins,
				DebugLoc dl, SelectionDAG &DAG,
				SmallVectorImpl<SDValue> &InVals) const;
		virtual SDValue
			LowerFormalArguments(SDValue Chain,
				CallingConv::ID CallConv, bool isVarArg,
				const SmallVectorImpl<ISD::InputArg> &Ins,
				DebugLoc dl, SelectionDAG &DAG,
				SmallVectorImpl<SDValue> &InVals) const;
		virtual SDValue
			LowerCall(SDValue Chain, SDValue Callee, CallingConv::ID CallConv,
				bool isVarArg, bool doesNotRet, bool &isTailCall,
				const SmallVectorImpl<ISD::OutputArg> &Outs,
				const SmallVectorImpl<SDValue> &OutVals,
				const SmallVectorImpl<ISD::InputArg> &Ins,
				DebugLoc dl, SelectionDAG &DAG,
				SmallVectorImpl<SDValue> &InVals) const;
		virtual SDValue
			LowerReturn(SDValue Chain,
				CallingConv::ID CallConv, bool isVarArg,
				const SmallVectorImpl<ISD::OutputArg> &Outs,
				const SmallVectorImpl<SDValue> &OutVals,
				DebugLoc dl, SelectionDAG &DAG) const;
	}; // end class Z80TargetLowering
} // end namespace llvm

#endif

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
#include "llvm\CodeGen\SelectionDAGISel.h"
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
	private:
		SDNode *Select(SDNode *N);
	}; // end class Z80DAGToDAGISel
} // end namespace

FunctionPass *llvm::createZ80ISelDag(Z80TargetMachine &TM, CodeGenOpt::Level OptLevel)
{
	return new Z80DAGToDAGISel(TM, OptLevel);
}

SDNode *Z80DAGToDAGISel::Select(SDNode *N)
{
	assert(0 && "Not Implemented yet!");
	return NULL;
}

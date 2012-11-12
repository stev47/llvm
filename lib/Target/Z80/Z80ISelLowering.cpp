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
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
using namespace llvm;

Z80TargetLowering::Z80TargetLowering(Z80TargetMachine &TM)
  : TargetLowering(TM, new TargetLoweringObjectFileELF())
{
  addRegisterClass(MVT::i8, &Z80::GR8RegClass);
  addRegisterClass(MVT::i16, &Z80::GR16RegClass);

  computeRegisterProperties();

  setBooleanContents(ZeroOrOneBooleanContent);

  setLoadExtAction(ISD::EXTLOAD, MVT::i1, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1, Promote);

  setOperationAction(ISD::ZERO_EXTEND, MVT::i16, Custom);
  setOperationAction(ISD::SIGN_EXTEND, MVT::i16, Custom);

  setOperationAction(ISD::SRL, MVT::i8, Custom);
  setOperationAction(ISD::SHL, MVT::i8, Custom);
  setOperationAction(ISD::SRA, MVT::i8, Custom);
  setOperationAction(ISD::SELECT, MVT::i8, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i8, Custom);
  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);
  setOperationAction(ISD::BR_CC, MVT::i8, Custom);
  //setOperationAction(ISD::STORE, MVT::i16, Custom);

  setOperationAction(ISD::AND, MVT::i16, Custom);
  setOperationAction(ISD::OR,  MVT::i16, Custom);
  setOperationAction(ISD::XOR, MVT::i16, Custom);

  setOperationAction(ISD::MUL, MVT::i8, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i8, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i8, Expand);
  setOperationAction(ISD::MUL, MVT::i16, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i16, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i16, Expand);

  setOperationAction(ISD::UDIV, MVT::i8, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i8, Expand);
  setOperationAction(ISD::SDIV, MVT::i8, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i8, Expand);
  setOperationAction(ISD::UDIV, MVT::i16, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i16, Expand);
  setOperationAction(ISD::SDIV, MVT::i16, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i16, Expand);

  setLibcallName(RTLIB::MUL_I8, "_mathLib_MULi8");
  setLibcallName(RTLIB::MUL_I16, "_mathLib_MULi16");
  setLibcallName(RTLIB::UDIV_I8, "_mathLib_UDIVi8");
  setLibcallName(RTLIB::UDIV_I16, "_mathLib_UDIVi16");
  setLibcallName(RTLIB::SDIV_I8, "_mathLib_SDIVi8");
  setLibcallName(RTLIB::SDIV_I16, "_mathLib_SDIVi16");

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
        VReg = MRI.createVirtualRegister(&Z80::GR16RegClass);
        MRI.addLiveIn(VA.getLocReg(), VReg);
        ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
        InVals.push_back(ArgValue);
        break;
      case MVT::i8:
        VReg = MRI.createVirtualRegister(&Z80::GR8RegClass);
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
  case Z80ISD::WRAPPER:   return "Z80ISD::WRAPPER";
  case Z80ISD::CALL:      return "Z80ISD::CALL";
  case Z80ISD::RET:       return "Z80ISD::RET";
  case Z80ISD::SELECT_CC: return "Z80ISD::SELECT_CC";
  case Z80ISD::BR_CC:     return "Z80ISD::BR_CC";
  case Z80ISD::CP:        return "Z80ISD::CP";
  case Z80ISD::RLC:       return "Z80ISD::RLC";
  case Z80ISD::RRC:       return "Z80ISD::RRC";
  case Z80ISD::RL:        return "Z80ISD::RL";
  case Z80ISD::RR:        return "Z80ISD::RR";
  case Z80ISD::SLA:       return "Z80ISD::SLA";
  case Z80ISD::SRA:       return "Z80ISD::SRA";
  case Z80ISD::SLL:       return "Z80ISD::SLL";
  case Z80ISD::SRL:       return "Z80ISD::SRL";
  }
}

SDValue Z80TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode()) {
  case ISD::STORE:         return LowerStore(Op, DAG);
  case ISD::SELECT_CC:     return LowerSelectCC(Op, DAG);
  case ISD::SRL:
  case ISD::SHL:
  case ISD::SRA:           return LowerShifts(Op, DAG);
  case ISD::GlobalAddress: return LowerGlobalAddress(Op, DAG);
  case ISD::BR_CC:         return LowerBrCC(Op, DAG);
  case ISD::ZERO_EXTEND:   return LowerZExt(Op, DAG);
  case ISD::SIGN_EXTEND:   return LowerSExt(Op, DAG);
  case ISD::AND:
  case ISD::OR:
  case ISD::XOR:           return LowerBinaryOp(Op, DAG);
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
  switch (BasePtr.getOpcode())
  {
  default:
    llvm_unreachable("LowerStore suppory only FrameIndex and CopyFromReg %IX, %IY");
  case ISD::FrameIndex:
    break;
  case ISD::CopyFromReg:
    SDValue Reg = BasePtr.getOperand(1);
    if (Reg != DAG.getRegister(Z80::IX, MVT::i16) &&
      Reg != DAG.getRegister(Z80::IY, MVT::i16))
      llvm_unreachable("Support CopyFromReg only from %IX, %IY");
    break;
  }
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

static SDValue EmitCMP(SDValue &LHS, SDValue &RHS, SDValue &TargetCC,
  ISD::CondCode CC,
  DebugLoc dl, SelectionDAG &DAG)
{
  assert(!LHS.getValueType().isFloatingPoint() && "We don't handle FP yet");

  Z80::CondCode TCC = Z80::COND_INVALID;
  switch (CC)
  {
  case ISD::SETUNE:
  case ISD::SETNE: // aka COND_NZ
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    TCC = Z80::COND_NZ;
    break;
  case ISD::SETUEQ:
  case ISD::SETEQ: // aka COND_Z
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    TCC = Z80::COND_Z;
    break;
  case ISD::SETUGT:
    std::swap(LHS, RHS);
  case ISD::SETULT: // aka COND_C
    TCC = Z80::COND_C;
    break;
  case ISD::SETULE:
    std::swap(LHS, RHS);
  case ISD::SETUGE: // aka COND_NC
    TCC = Z80::COND_NC;
    break;
  default: llvm_unreachable("Invalid integer condition!");
  }
  TargetCC = DAG.getConstant(TCC, MVT::i8);
  return DAG.getNode(Z80ISD::CP, dl, MVT::Glue, LHS, RHS);
}

SDValue Z80TargetLowering::LowerSelectCC(SDValue Op, SelectionDAG &DAG) const
{
  SDValue LHS      = Op.getOperand(0);
  SDValue RHS      = Op.getOperand(1);
  SDValue TrueV    = Op.getOperand(2);
  SDValue FalseV   = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  DebugLoc dl      = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SmallVector<SDValue, 4> Ops;
  Ops.push_back(TrueV);
  Ops.push_back(FalseV);
  Ops.push_back(TargetCC);
  Ops.push_back(Flag);

  return DAG.getNode(Z80ISD::SELECT_CC, dl, VTs, &Ops[0], Ops.size());
}

EVT Z80TargetLowering::getSetCCResultType(EVT VT) const
{
  if (!VT.isVector()) return MVT::i8;
  return VT.changeVectorElementTypeToInteger();
}

MachineBasicBlock* Z80TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
  MachineBasicBlock *MBB) const
{
  unsigned Opc = MI->getOpcode();
  DebugLoc dl = MI->getDebugLoc();
  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();

  assert((Opc == Z80::SELECT8) && "Unexpected instr type to insert");

  const BasicBlock *LLVM_BB = MBB->getBasicBlock();
  MachineFunction::iterator I = MBB;
  I++;

  MachineBasicBlock *thisMBB = MBB;
  MachineFunction *MF = MBB->getParent();
  MachineBasicBlock *copy0MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *copy1MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MF->insert(I, copy0MBB);
  MF->insert(I, copy1MBB);

  copy1MBB->splice(copy1MBB->begin(), MBB,
    llvm::next(MachineBasicBlock::iterator(MI)), MBB->end());
  copy1MBB->transferSuccessorsAndUpdatePHIs(MBB);
  MBB->addSuccessor(copy0MBB);
  MBB->addSuccessor(copy1MBB);

  BuildMI(MBB, dl, TII.get(Z80::JPCC))
    .addMBB(copy1MBB)
    .addImm(MI->getOperand(3).getImm());

  MBB = copy0MBB;
  MBB->addSuccessor(copy1MBB);

  MBB = copy1MBB;
  BuildMI(*MBB, MBB->begin(), dl, TII.get(Z80::PHI),
    MI->getOperand(0).getReg())
    .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB)
    .addReg(MI->getOperand(1).getReg()).addMBB(thisMBB);

  MI->eraseFromParent();
  return MBB;
}

SDValue Z80TargetLowering::LowerShifts(SDValue Op, SelectionDAG &DAG) const
{
  unsigned Opc = Op.getOpcode();
  SDNode *N = Op.getNode();
  EVT VT = Op.getValueType();
  DebugLoc dl = Op.getDebugLoc();

  assert((VT == MVT::i8) && "This type of shifts is not implemented yet!");
  assert(isa<ConstantSDNode>(N->getOperand(1)) && "Non constant shifts are not implemented yet!");

  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
  SDValue Victim = N->getOperand(0);

  switch (Opc)
  {
  default: llvm_unreachable("Invalid shift opcode");
  case ISD::SRL:
    Opc = Z80ISD::SRL;
    break;
  case ISD::SHL:
    Opc = Z80ISD::SLA;
    break;
  case ISD::SRA:
    Opc = Z80ISD::SRA;
    break;
  }

  while (ShiftAmount--)
    Victim = DAG.getNode(Opc, dl, VT, Victim);

  return Victim;
}

SDValue Z80TargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op->getDebugLoc();
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();
  SDValue Result = DAG.getTargetGlobalAddress(GV, dl, getPointerTy(), Offset);
  return DAG.getNode(Z80ISD::WRAPPER, dl, getPointerTy(), Result);
}

SDValue Z80TargetLowering::LowerBrCC(SDValue Op, SelectionDAG &DAG) const
{
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS   = Op.getOperand(2);
  SDValue RHS   = Op.getOperand(3);
  SDValue Dest  = Op.getOperand(4);
  DebugLoc dl   = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  return DAG.getNode(Z80ISD::BR_CC, dl, Op.getValueType(), Chain, Dest, TargetCC, Flag);
}

SDValue Z80TargetLowering::LowerZExt(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();

  assert(VT == MVT::i16 && "ZExt support only i16");
  
  SDValue Tmp = SDValue(DAG.getMachineNode(Z80::LD8ri, dl, MVT::i8, DAG.getTargetConstant(0, MVT::i8)), 0);
  SDValue HI  = DAG.getTargetInsertSubreg(Z80::sub_8bit_hi, dl, VT, DAG.getUNDEF(VT), Tmp);
  SDValue LO  = DAG.getTargetInsertSubreg(Z80::sub_8bit_low, dl, VT, HI, Val);
  return LO;
}

SDValue Z80TargetLowering::LowerSExt(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();

  assert(VT == MVT::i16 && "SExt support only i16");

  SDValue Flag, Tmp, LO, HI;
  SDVTList VTs = DAG.getVTList(MVT::i8, MVT::Glue);
  SmallVector<SDValue, 2> Ops;

  LO   = DAG.getTargetInsertSubreg(Z80::sub_8bit_low, dl, VT, DAG.getUNDEF(VT), Val);

  Ops.push_back(Val);
  Tmp  = SDValue(DAG.getMachineNode(Z80::RL8r, dl, VTs, &Ops[0], Ops.size()), 1);

  Ops.clear();
  Ops.push_back(DAG.getRegister(Z80::A, MVT::i8));
  Ops.push_back(Tmp);
  Flag = SDValue(DAG.getMachineNode(Z80::SBC8r, dl, VTs, &Ops[0], Ops.size()), 0);

  HI   = DAG.getTargetInsertSubreg(Z80::sub_8bit_hi, dl, VT, LO, Flag);
  return HI;
}

SDValue Z80TargetLowering::LowerBinaryOp(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  EVT VT      = Op.getValueType();
  EVT HalfVT  = VT.getHalfSizedIntegerVT(*DAG.getContext());

  assert(VT == MVT::i16 && "Invalid type for lowering");

  unsigned Opc = Op.getOpcode();

  SDValue LHS_LO, LHS_HI;
  SDValue RHS_LO, RHS_HI;
  SDValue LO, HI;
  SDValue Tmp1, Tmp2;

  LHS_LO = DAG.getTargetExtractSubreg(Z80::sub_8bit_low, dl, HalfVT, LHS);
  LHS_HI = DAG.getTargetExtractSubreg(Z80::sub_8bit_hi,  dl, HalfVT, LHS);
  if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(RHS)) {
    RHS_LO = DAG.getConstant(CN->getZExtValue() & 0xFF, HalfVT);
    RHS_HI = DAG.getConstant(CN->getZExtValue()>>8 & 0xFF, HalfVT);
  } else {
    RHS_LO = DAG.getTargetExtractSubreg(Z80::sub_8bit_low, dl, HalfVT, RHS);
    RHS_HI = DAG.getTargetExtractSubreg(Z80::sub_8bit_hi,  dl, HalfVT, RHS);
  }

  LO = DAG.getNode(Opc, dl, HalfVT, LHS_LO, RHS_LO);
  HI = DAG.getNode(Opc, dl, HalfVT, LHS_HI, RHS_HI);

  Tmp1 = DAG.getTargetInsertSubreg(Z80::sub_8bit_low, dl, VT, DAG.getUNDEF(VT), LO);
  Tmp2 = DAG.getTargetInsertSubreg(Z80::sub_8bit_hi,  dl, VT, Tmp1, HI);

  return Tmp2;
}

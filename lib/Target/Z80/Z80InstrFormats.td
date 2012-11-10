//===-- Z80InstrFormats.td - Z80 Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions.
//===----------------------------------------------------------------------===//

class Format<bits<3> val> {
  bits<3> Value = val;
}

// Naming of opcode bits encoding is borrowed from this page:
// http://z80.info/decoding.htm
// 7 6 5 4 3 2 1 0
// x x y y y z z z
//     p p q

def FPseudo : Format<0>;
def FRaw    : Format<1>;
def FRy     : Format<2>; // r[y] = B, C, D, E, H, L, (HL)(excluded), A
def FRz     : Format<3>; // r[z]
def FRR     : Format<4>; // r[y], r[z] - two 8-bit registers
def FRP     : Format<5>; // rp[p] = BC/DE/HL/SP

class ImmType<bits<2> val> {
  bits<2> Value = val;
}
def NoImm     : ImmType<0>;
def Imm8      : ImmType<1>;
def Imm8PCRel : ImmType<2>;
def Imm16     : ImmType<3>;

// 3 bits encoding rotation/shift operation type
class RSOpType<bits<3> val, string node> {
  bits<3> Value = val;
  string Node = node;
}

def RS_RLC : RSOpType<0, "Z80rlc">;
def RS_RRC : RSOpType<1, "Z80rrc">;
def RS_RL  : RSOpType<2, "Z80rl">;
def RS_RR  : RSOpType<3, "Z80rr">;
def RS_SLA : RSOpType<4, "Z80sla">;
def RS_SRA : RSOpType<5, "Z80sra">;
def RS_SLL : RSOpType<6, "Z80sll">;
def RS_SRL : RSOpType<7, "Z80srl">;

// 3 bits encoding ALU operation type
class ALUOpType<bits<3> val, string node> {
  bits<3> Value = val;
  string Node = node;
}

def ALU_ADD : ALUOpType<0, "add">;
def ALU_ADC : ALUOpType<1, "adde">;
def ALU_SUB : ALUOpType<2, "sub">;
def ALU_SBC : ALUOpType<3, "sube">;
def ALU_AND : ALUOpType<4, "and">;
def ALU_XOR : ALUOpType<5, "xor">;
def ALU_OR  : ALUOpType<6, "or">;
def ALU_CP  : ALUOpType<7, "Z80cp">;

class Z80Pseudo<Format f, ImmType i, dag outs, dag ins>
  : Instruction {
  let Namespace = "Z80";

  Format Form = f;
  ImmType ImmT = i;

  dag OutOperandList = outs;
  dag InOperandList = ins;

  let TSFlags{2-0} = Form.Value;
  let TSFlags{4-3} = ImmT.Value;
}

class Z80Inst<bits<8> opc, Format f, ImmType i, dag outs, dag ins,
              string asmstr>
  : Z80Pseudo<f, i, outs, ins> {
  bits<8> Opcode = opc;
  let AsmString = asmstr;
}

class PseudoI<dag outs, dag ins, list<dag> pattern>
  : Z80Pseudo<FPseudo, NoImm, outs, ins> {
  let Pattern = pattern;
}

class I<bits<8> opc, Format f, dag outs, dag ins, string asmstr,
        list<dag> pattern>
  : Z80Inst<opc, f, NoImm, outs, ins, asmstr> {
  let Pattern = pattern;
  let CodeSize = 1;
}

class Ii8<bits<8> opc, Format f, dag outs, dag ins, string asmstr,
          list<dag> pattern>
  : Z80Inst<opc, f, Imm8, outs, ins, asmstr> {
  let Pattern = pattern;
  let CodeSize = 2;
}

class Ii16<bits<8> opc, Format f, dag outs, dag ins, string asmstr,
           list<dag> pattern>
  : Z80Inst<opc, f, Imm16, outs, ins, asmstr> {
  let Pattern = pattern;
  let CodeSize = 3;
}

class ALU<bits<8> opc, ALUOpType alu, Format f, ImmType i, dag outs, dag ins,
           string asmstr>
  : Z80Inst<opc, f, i, outs, ins, asmstr> {
  ALUOpType aluType = alu;
  let Opcode{3-5} = aluType.Value;
  let CodeSize = 1;
  let Uses = [A];
}

class ALUIr<ALUOpType alu, string asmstr>
  : ALU<0x80, alu, FRz, NoImm, (outs), (ins GR8:$src), asmstr> {
  let Pattern = !if(!eq(!cast<string>(aluType), "ALU_CP"),
    [(!cast<dag>(aluType.Node) A, GR8:$src), (implicit FLAGS)],
    [(set A, (!cast<dag>(aluType.Node) A, GR8:$src)), (implicit FLAGS)]);
  let Defs = !if(!eq(!cast<string>(aluType), "ALU_CP"),
    [FLAGS],
    [A, FLAGS]);
}

class ALUIi<ALUOpType alu, string asmstr>
  : ALU<0xC6, alu, FRaw, Imm8, (outs), (ins i8imm:$src), asmstr> {
  let Pattern = !if(!eq(!cast<string>(aluType), "ALU_CP"),
    [(!cast<dag>(aluType.Node) A, imm:$src), (implicit FLAGS)],
    [(set A, (!cast<dag>(aluType.Node) A, imm:$src)), (implicit FLAGS)]);
  let Defs = !if(!eq(!cast<string>(aluType), "ALU_CP"),
    [FLAGS],
    [A, FLAGS]);
}

multiclass ALUI<ALUOpType alu, string asmstr> {
  def r : ALUIr<alu, asmstr>;
  def i : ALUIi<alu, asmstr>;
}

class RSIr<RSOpType rs, string asmstr>
  : Z80Inst<0x00, FRz, NoImm, (outs GR8:$dst), (ins GR8:$src), asmstr> {
  RSOpType rsType = rs;
  let Opcode{5-3} = rsType.Value;
  let CodeSize = 2;
  let Pattern = [(set GR8:$dst, (!cast<dag>(rsType.Node) GR8:$src))];
  let Defs = [FLAGS];
  let Constraints = "$src = $dst";
}

multiclass RSI<RSOpType rs, string asmstr> {
  def r : RSIr<rs, asmstr>;
}

set(LLVM_TARGET_DEFINITIONS Z80.td)

tablegen(LLVM Z80GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Z80GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Z80GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(Z80CommonTableGen)

add_llvm_target(Z80CodeGen
    Z80ISelLowering.cpp
    Z80FrameLowering.cpp
    Z80RegisterInfo.cpp
    Z80InstrInfo.cpp
    Z80SelectionDAGInfo.cpp
    Z80Subtarget.cpp
    Z80TargetMachine.cpp
)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)

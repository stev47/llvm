//====-- Z80Subtarget.h - Define Subtarget for the Z80 ---------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Z80 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef Z80SUBTARGET_H
#define Z80SUBTARGET_H

#include "llvm/Target/TargetSubtargetInfo.h"
#include "llvm/MC/MCInstrItineraries.h"

#define GET_SUBTARGETINFO_HEADER
#include "Z80GenSubtargetInfo.inc"

#include <string>

namespace llvm {
    class StringRef;
    
    class Z80Subtarget : public Z80GenSubtargetInfo {
    public:
        // This constructor initializes the data members to match that
        // of the specified triple.
        //
        Z80Subtarget(const std::string &TT, const std::string &CPU,
                     const std::string &FS);

        // ParseSubtargetFeatures - Parses features string setting specified
        // subtarget options. Definition of function is auto generated
        // by tblgen.
        void ParseSubtargetFeatures(StringRef CPU, StringRef FS);
    }; // end Z80Subtarget
} // end namespace llvm

#endif

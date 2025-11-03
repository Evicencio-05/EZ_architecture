#pragma once

#include <cstdint>
#include <string>

namespace ez_arch {

// Basic MIPS types
using word_t = uint32_t;      // 32-bit word
using address_t = uint32_t;   // Memory address
using register_id_t = uint8_t; // Register identifier (0-31)

// Instruction formats
enum class InstructionFormat {
    R_TYPE,  // Register format
    I_TYPE,  // Immediate format
    J_TYPE   // Jump format
};

// Opcode enumeration (subset of MIPS instructions)
enum class Opcode : uint8_t {
    // R-type (opcode = 0)
    ADD = 0x20,
    SUB = 0x22,
    AND = 0x24,
    OR = 0x25,
    SLT = 0x2A,
    // I-type
    ADDI = 0x08,
    ANDI = 0x0C,
    ORI = 0x0D,
    LW = 0x23,
    SW = 0x2B,
    BEQ = 0x04,
    BNE = 0x05,
    // J-type
    J = 0x02,
    JAL = 0x03
};

} // namespace ez_arch

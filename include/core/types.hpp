#pragma once

#include <cstdint>
#include <string>

namespace ez_arch {

using word_t = uint32_t;
using address_t = uint32_t;
using register_id_t = uint8_t;

enum class InstructionFormat {
    R_TYPE,
    I_TYPE,
    J_TYPE 
};

namespace Opcode {
    constexpr uint8_t ADDI = 0x08;
    constexpr uint8_t ANDI = 0x0C;
    constexpr uint8_t ORI = 0x0D;
    constexpr uint8_t LW = 0x23;
    constexpr uint8_t SW = 0x2B;
    constexpr uint8_t BEQ = 0x04;
    constexpr uint8_t BNE = 0x05;
    constexpr uint8_t J = 0x02;
    constexpr uint8_t JAL = 0x03;
}

namespace Funct {
    constexpr uint8_t ADD = 0x20;
    constexpr uint8_t SUB = 0x22;
    constexpr uint8_t AND = 0x24;
    constexpr uint8_t OR = 0x25;
    constexpr uint8_t SLT = 0x2A;
}

} // namespace ez_arch

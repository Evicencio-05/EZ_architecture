#pragma once

#include <cstdint>

namespace ez_arch {

using word_t = uint32_t;
using address_t = uint32_t;
using register_id_t = uint8_t;

enum class InstructionFormat : uint8_t { R_TYPE, I_TYPE, J_TYPE };

namespace Opcode {
constexpr uint8_t kADDI = 0x08;
constexpr uint8_t kANDI = 0x0C;
constexpr uint8_t kORI = 0x0D;
constexpr uint8_t kLW = 0x23;
constexpr uint8_t kSW = 0x2B;
constexpr uint8_t kBEQ = 0x04;
constexpr uint8_t kBNE = 0x05;
constexpr uint8_t kJ = 0x02; // NOLINT
constexpr uint8_t kJAL = 0x03;
} // namespace Opcode

namespace Funct {
constexpr uint8_t kADD = 0x20;
constexpr uint8_t kSUB = 0x22;
constexpr uint8_t kAND = 0x24;
constexpr uint8_t kOR = 0x25;
constexpr uint8_t kSLT = 0x2A;
} // namespace Funct

} // namespace ez_arch

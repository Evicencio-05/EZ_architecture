#include "core/memory.hpp"

#include <algorithm>
#include <cassert>

namespace ez_arch {

void Memory::checkAlignment(address_t addr) {
  assert((addr & 0x3) == 0);
}

void Memory::checkBounds(address_t addr, size_t accessSize) const {
  assert((addr + accessSize) <= m_memory.size());
}

word_t Memory::readWord(address_t addr) const {
  checkAlignment(addr);
  checkBounds(addr, kWORD_ACCESS_SIZE);
  word_t result =
      (m_memory[addr] << 24) | (m_memory[addr + 1] << 16)
      | (m_memory[addr + 2] << 8)
      | (m_memory[addr + 3]); // Combine bytes into word (big endian)
  return result;
}

void Memory::writeWord(address_t addr, word_t value) {
  checkAlignment(addr);
  checkBounds(addr, kWORD_ACCESS_SIZE);
  m_memory[addr] = (value >> 24) & 0xFF;
  m_memory[addr + 1] = (value >> 16) & 0xFF;
  m_memory[addr + 2] = (value >> 8) & 0xFF;
  m_memory[addr + 3] = value & 0xFF;
}

uint8_t Memory::readByte(address_t addr) const {
  checkBounds(addr, kBYTE_ACCESS_SIZE);
  return m_memory[addr];
}

void Memory::writeByte(address_t addr, uint8_t value) {
  checkBounds(addr, kBYTE_ACCESS_SIZE);
  m_memory[addr] = value;
}

void Memory::loadProgram(const std::vector<word_t>& program,
                         address_t startAddr) {
  const size_t kPROGRAM_SIZE = program.size();

  checkBounds(startAddr, kPROGRAM_SIZE * kWORD_ACCESS_SIZE);
  checkAlignment(startAddr);

  // For overflow wrapping:
  // size_t bytes_needed = program_size * WORD_ACCESS_SIZE; // Check if program
  // fits assert(bytes_needed / WORD_ACCESS_SIZE == program_size); // Overflow
  // check assert(start_addr + bytes_needed <= m_memory.size());     // Bounds
  // check

  for (size_t i = 0; i < kPROGRAM_SIZE; ++i) {
    writeWord(startAddr + (i * 4), program[i]);
  }
}

void Memory::reset() {
  std::fill(m_memory.begin(), m_memory.end(), 0);
}

} // namespace ez_arch

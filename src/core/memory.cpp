#include <cassert>
#include <algorithm>

#include "core/memory.hpp"

namespace ez_arch {
    
    void Memory::check_alignment(address_t addr) const {
        assert((addr & 0x3) == 0);
    }

    void Memory::check_bounds(address_t addr, size_t access_size) const {
        assert((addr + access_size) <= memory_.size());
    }

    word_t Memory::read_word(address_t addr) const {
      check_alignment(addr);
      check_bounds(addr, WORD_ACCESS_SIZE);
      word_t result = (memory_[addr] << 24)     |
                      (memory_[addr + 1] << 16) |
                      (memory_[addr + 2] << 8)  |
                      (memory_[addr + 3]); // Combine bytes into word (big endian)
      return result;
    }

    void Memory::write_word(address_t addr, word_t value) {
      check_alignment(addr);
      check_bounds(addr, WORD_ACCESS_SIZE);
      memory_[addr]     = (value >> 24) & 0xFF;
      memory_[addr + 1] = (value >> 16) & 0xFF;
      memory_[addr + 2] = (value >> 8) & 0xFF;
      memory_[addr + 3] = value & 0xFF;
    }
    
    uint8_t Memory::read_byte(address_t addr) const {
      check_bounds(addr, BYTE_ACCESS_SIZE);
      return memory_[addr];
    }

    void Memory::write_byte(address_t addr, uint8_t value) {
      check_bounds(addr, BYTE_ACCESS_SIZE);
      memory_[addr] = value;
    }
    
    void Memory::load_program(const std::vector<word_t>& program, address_t start_addr){ 
      const size_t program_size = program.size();
      
      check_bounds(start_addr, program_size * WORD_ACCESS_SIZE);
      check_alignment(start_addr);

      // For overflow wrapping:
      // size_t bytes_needed = program_size * WORD_ACCESS_SIZE; // Check if program fits
      // assert(bytes_needed / WORD_ACCESS_SIZE == program_size); // Overflow check
      // assert(start_addr + bytes_needed <= memory_.size());     // Bounds check

      for (size_t i = 0; i < program_size; ++i) {
        write_word(start_addr + (i * 4), program[i]);
      }
    }

    void Memory::reset() {
      std::fill(memory_.begin(), memory_.end(), 0);
    }

} // namespace ez_arch

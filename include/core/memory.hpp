#pragma once

#include "types.hpp"
#include <vector>

namespace ez_arch {

class Memory {
public:
    explicit Memory(size_t size = 1024 * 1024) :
      memory_(size, 0) {} // Default 1MB
    
    word_t read_word(address_t addr) const;
    void write_word(address_t addr, word_t value);
    
    uint8_t read_byte(address_t addr) const;
    void write_byte(address_t addr, uint8_t value);
    
    void load_program(const std::vector<word_t>& program, address_t start_addr = 0);
    void reset();
    
    size_t size() const { return memory_.size(); }

    static constexpr size_t BYTE_ACCESS_SIZE = 1;
    static constexpr size_t WORD_ACCESS_SIZE = 4;
    
private:
    std::vector<uint8_t> memory_;
    
    void check_alignment(address_t addr) const;
    void check_bounds(address_t addr, size_t access_size) const;
};

} // namespace ez_arch

#pragma once

#include "types.hpp"
#include <vector>

namespace ez_arch {

class Memory {
public:
    explicit Memory(size_t size = 1024 * 1024) :
      m_memory(size, 0) {} // Default 1MB
    
    [[nodiscard]] word_t readWord(address_t addr) const;
    void writeWord(address_t addr, word_t value);
    
    [[nodiscard]] uint8_t readByte(address_t addr) const;
    void writeByte(address_t addr, uint8_t value);
    
    void loadProgram(const std::vector<word_t>& program, address_t startAddr = 0);
    void reset();
    
    [[nodiscard]] size_t size() const { return m_memory.size(); }

    static constexpr size_t kBYTE_ACCESS_SIZE = 1;
    static constexpr size_t kWORD_ACCESS_SIZE = 4;
    
private:
    std::vector<uint8_t> m_memory;
    
    static void checkAlignment(address_t addr) ;
    void checkBounds(address_t addr, size_t accessSize) const;
};

} // namespace ez_arch

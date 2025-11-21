#include "core/register_file.hpp"
#include <cassert>

namespace ez_arch {

RegisterFile::RegisterFile() {
    m_registers.fill(0);
    m_pc = 0;
}

word_t RegisterFile::read(register_id_t reg) const {
    assert(reg < kNUM_REGISTERS);
    return m_registers[reg];
}

void RegisterFile::write(register_id_t reg, word_t value) {
    assert(reg < kNUM_REGISTERS);
    if (reg == 0) { return;
}
    m_registers[reg] = value;
}

void RegisterFile::reset() {
    m_registers.fill(0);
    m_pc = 0;
}

} // namespace ez_arch

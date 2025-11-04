#include "core/register_file.hpp"
#include <cassert>
#include <algorithm>

namespace ez_arch {

RegisterFile::RegisterFile() {
    registers_.fill(0);
    pc_ = 0;
}

word_t RegisterFile::read(register_id_t reg) const {
    assert(reg < NUM_REGISTERS);
    return registers_[reg];
}

void RegisterFile::write(register_id_t reg, word_t value) {
    assert(reg < NUM_REGISTERS);
    if (reg == 0) return;
    registers_[reg] = value;
}

void RegisterFile::reset() {
    registers_.fill(0);
    pc_ = 0;
}

} // namespace ez_arch

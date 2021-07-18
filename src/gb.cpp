#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = Memory.read(Cpu.pc);
    Cpu.execute(opcode);
}

void gb::runFrame() {
    fmt::print ("Frame {}\n", framesPassed++);
}
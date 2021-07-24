#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = Cpu.Fetch8();
    Cpu.execute(opcode);
}

void gb::runFrame() {
    fmt::print ("Frame {}\n", framesPassed++);
}
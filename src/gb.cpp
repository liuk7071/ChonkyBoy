#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = Cpu.Fetch8();
    Cpu.execute(opcode);
}

void gb::runFrame() {
    framesPassed++;
    while(Cpu.frame_cycles < 70224) {
        step();
    }
    Cpu.frame_cycles = 0;
}